// dm-server/src/game/PvP/PvPSystem.h
// Ядро PvP: флаги, зоны (pvp_zones), дуэли, репутационные штрафы.
// Контракт битов совпадает с proto/opcodes.defs v1.7 и EPvPFlag клиента.
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace dm {

/// Битовая маска PvP-флагов. СЕРВЕР — источник истины; клиент только рендерит.
/// Синхронизировано с sql/upgrades/PvE_PvP_Core.sql (character_flags.pvp_flags).
enum PvpFlags : uint16_t {
    PVP_FLAGGED   = 1u << 0, // ручной/по атаке флаг, таймер авто-снятия
    FFA           = 1u << 1, // зона «Ржавые пустоши»: все атакуемы
    IN_SANCTUARY  = 1u << 2, // город: атаки запрещены
    DUELING       = 1u << 3, // активная дуэль
    WANTED        = 1u << 4, // PvP Bounty (wanted_level > 0)
    DISHONORED    = 1u << 5, // запрет вступать в группы (Dishonorable Death)
};

enum ZoneRuleSet : uint8_t {
    RULESET_SANCTUARY = 1,
    RULESET_ARENA     = 2,
    RULESET_FFA       = 3,
};

struct PvpZoneInfo {
    uint16_t mapId = 0;
    uint16_t areaId = 0;
    std::string name;
    ZoneRuleSet ruleset = RULESET_SANCTUARY;
    bool forcedUntag = true;
    bool autoResurrect = false;
    bool cleanDebuffs = false;
    uint32_t honorMultQ16 = 65536u; // x1.0
};

struct DuelState {
    uint64_t duelId = 0;
    uint32_t initiatorGuid = 0;
    uint32_t targetGuid = 0;
    int64_t startedMs = 0;
};

/// Таймауты/пороги — единый баланс-контракт с клиентом (CombatHUD 6с декей).
struct PvpConfig {
    uint32_t combatTimeoutSec = 6;      // SMSG_COMBAT_STATE: снятие флага через 6с после последнего хита
    uint32_t flagMinDurationSec = 15;   // флаг не снимается раньше этого срока (анти-чит-отрицание)
    uint32_t dishonorLevelGap = 10;     // victim слабее на >10 ур -> Dishonorable
    uint32_t dishonorGroupBanSec = 7200;
    uint32_t wantedKillsNoDeath = 5;    // 5 убийств без смерти -> +1 wanted level (cap 5)
    int32_t repLossSameFaction = -75;   // убийство софракциона без флага
    int32_t honorBaseKill = 100;
};

class PvPSystem {
public:
    static PvPSystem &Instance();

    /// Загрузка pvp_zones из dm_world (SELECT * FROM pvp_zones).
    /// Вызывается при старте mapserver/worldserver.
    void LoadZoneCache(/* SqlResult rows */ const std::vector<PvpZoneInfo> &rows);

    const PvpZoneInfo *FindZone(uint16_t mapId, uint16_t areaId) const;

    // --- Механика включения флагов (источники) ---
    /// CMSG_PVP_TOGGLE 0x2040: war mode toggle. В sanctuary — отказ.
    bool OnPvpCommand(uint32_t guid, bool enable);
    /// Смена зоны игроком (OnEnterZone): применяет правила зоны, шлёт
    /// SMSG_PVP_ZONE_INFO 0x2042 + SMSG_PVP_FLAG_UPDATE 0x2041.
    void OnEnterZone(uint32_t guid, uint16_t mapId, uint16_t areaId);
    /// Атака по игроку: автофлаг атакующего (если цель не sanctuary).
    bool OnAttackPlayer(uint32_t attackerGuid, uint32_t victimGuid);

    // --- Репутация за PvP (связь с ReputationMgr) ---
    /// Возвращает delta репутации: 0 для ренегата/врага/флагнутой цели своей
    /// фракции; cfg.repLossSameFaction для «убийства без флага своего».
    int32_t ComputeRepPenalty(bool sameFaction, bool victimRenegade, bool victimFlagged) const;

    /// Полный разбор PvP-убийства: honor (x zone mult), wanted level, dishonor.
    struct KillOutcome {
        uint16_t honorGain = 0;
        int32_t repDelta = 0;
        bool dishonored = false;
        uint8_t wantedLevel = 0;
    };
    KillOutcome OnPlayerKilledPlayer(uint32_t killerGuid, uint32_t victimGuid, uint16_t mapId,
                                     uint16_t areaId, bool sameFaction, bool victimRenegade,
                                     bool victimFlagged, uint8_t killerLevel, uint8_t victimLevel);

    // --- Дуэли ---
    uint64_t DuelRequest(uint32_t initiator, uint32_t target); // CMSG_DUEL_REQUEST 0x2043
    bool DuelAccept(uint64_t duelId, bool accept);             // CMSG_DUEL_ACCEPT 0x2045
    void DuelComplete(uint64_t duelId, uint32_t winnerGuid, uint8_t reason); // 0 killed 1 cancel 2 timeout

    // --- Проверки для ядра (используется MapServer/Combat) ---
    bool CanAttack(uint32_t attackerGuid, uint32_t victimGuid) const;
    uint16_t GetFlags(uint32_t guid) const {
        auto it = m_flags.find(guid);
        return it == m_flags.end() ? 0 : it->second;
    }

    /// Персист в character_flags (MySQL). Debounce внутри SQL-слоя.
    /// INSERT ... ON DUPLICATE KEY UPDATE pvp_flags=?, flag_expire_time=?, ...
    uint8_t GetWantedForSql(uint32_t guid) const;
    std::string BuildPersistSql(uint32_t guid) const;
    /// Строка лога pvp_kills_log (пишется в транзакции с ModifyReputation).
    std::string BuildKillLogSql(const KillOutcome &o, uint32_t killer, uint32_t victim,
                                uint16_t mapId, uint16_t areaId, uint8_t ruleset,
                                uint32_t ts) const;

    const PvpConfig &Config() const { return m_cfg; }

private:
    PvPSystem() = default;
    std::unordered_map<uint64_t, PvpZoneInfo> m_zones; // key = map<<16|area
    std::unordered_map<uint32_t, uint16_t> m_flags;
    std::unordered_map<uint32_t, uint8_t> m_wanted;
    std::unordered_map<uint64_t, DuelState> m_duels;
    uint64_t m_nextDuelId = 1;
    PvpConfig m_cfg;
};

} // namespace dm
