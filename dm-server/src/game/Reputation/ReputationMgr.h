// =====================================================================
// «Чёрная Материя» — ReputationMgr (аналог TrinityCore sql/reputation +
// кастомная механика «Ренегат»).
//
// Хранение: dm_characters.reputation (guid, faction_id, standing) — одна
// строка на фракцию; денормализованные «горячие» поля для быстрого PvP-роутинга
// и загрузки в CharacterHandler: characters.reputation_faction_1,
// characters.reputation_faction_2, characters.is_renegade (см. sql DB_Schema.sql).
//
// Standing: signed int32 в диапазоне [-60000, +42000] (как TRC), но шаг
// трактуется как fixed-point Q16: 1 единица = 1/16 очка репутации — чтобы
// Lua/C++ могли начислять дробные награды без float (детерминизм docs/02 §6).
// Границы рангов (Hated..Exalted) лежат в dbc (Faction.dbc), не в БД.
// =====================================================================
#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace DM {
class Player;

// Контракт с движком сервера (реализует CharacterDatabase / MapScripting):
// внедряется через Singleton, вызывается из Combat.cpp, QuestMgr.cpp,
// SpellMgr.cpp и HookMgr (docs/02).
struct RepChangeContext {
    enum class Source : uint8_t {
        KillCreature = 0, // убийство моба (SmartAI/Creature::KilledBy)
        QuestReward = 1,  // сдача квеста
        KillPlayer = 2,   // PvP-убийство игрока
        DungeonComplete = 3,
        Script = 4,            // прямой вызов из Lua (Eluna-style)
        SeepageCorruption = 5, // пассивное смещение от Истечения ЧМ
        FrontlineCapture = 6,
        GMCommand = 7,
    };

    Source source = Source::Script;
    uint32 triggerEntry = 0; // creature/quest id источника
    int32 rawAmount = 0;     // до множителей (для аудита)
};

enum FactionId : uint16_t {
    FACTION_NEUTRAL = 0,
    FACTION_ORDER_LIGHT = 1, // «Стражи Света» / Орден Рассвета (стартовая А)
    FACTION_FALL_DOWN = 2,   // «Падшие» / Круг Поглощения (стартовая Б)
    FACTION_THRESHOLD = 3,   // Гильдия Порога (нейтральная торгово-наёмническая)
    FACTION_RENEGADE = 4,    // служебная фракция изгоев (черный рынок, q_1003)
    FACTION_COUNT_KNOWN = 5,
};

enum RepRank : uint8_t {
    RANK_HATED = 0,      // [-60000, -36000)
    RANK_HOSTILE = 1,    // [-36000,  -9000)
    RANK_UNFRIENDLY = 2, // [ -9000,  -3000)
    RANK_NEUTRAL = 3,    // [ -3000,   3000)
    RANK_FRIENDLY = 4,   // [  3000,  18000)   <- порог gossip-опций (NPC_Config.sql)
    RANK_HONORED = 5,    // [ 18000,  39000)
    RANK_REVERED = 6,    // [ 39000,  42000]
    RANK_EXALTED = 7,    // == 42000 cap
};

// Пороги «Ренегата» (контракт с дизайн-доком 05-GDD §Фракции):
//   A (Орден Света) упала НИЖЕ -5000 И Б (Падшие) ниже 0 => Изгой.
inline constexpr int32 RENEGADE_THRESHOLD_A = -5000;
inline constexpr int32 RENEGADE_THRESHOLD_B = 0;

class ReputationMgr final {
public:
    static ReputationMgr &Instance();

    // ---- основной API (только с world-thread'а конкретного mapserver'а) ----
    // Изменяет репутацию игрока с учётом rate-множителей (rep_rate x1/x2/x4
    // из season_config), каскадно пересчитывает флаг ренегата и шлёт
    // SMSG_FACTION_STATE/SMSG_REP_UPDATE. Возвращает применённую (после
    // множителей) дельту; 0 если изменение проигнорировано (cap/ренегат-блок).
    int32 ModifyReputation(Player *player, uint16_t factionId, int32 amount,
                           const RepChangeContext &ctx);

    int32 GetReputation(const Player *player, uint16_t factionId) const;
    RepRank GetRank(const Player *player, uint16_t factionId) const;
    bool IsRenegate(const Player *player) const;

    // PvP-хук: вызывается из Unit::Kill для жертвы-игрока.
    // Правила: у ренегата НЕ списывается репутация ни с одной фракции;
    // убийство ренегата обеими фракциями даёт фикс. bounty (Splinters),
    // но не трогает standing. Возвращает true, если были применены изменения.
    bool OnPlayerKilledPlayer(Player *killer, Player *victim);

    // Пересчёт флага после внешних правок (GM, миграции, загрузка персона).
    void RecalcRenegateFlag(Player *player);

    // Загрузка/сохранение денормализованных колонок characters.*
    // (полный standing — в таблице reputation, hot path держим в памяти).
    void LoadFromDB(Player *player);
    void SaveToDB(Player *player) const;

    // Тик сохранений (как TRC CharacterSaver): раз в REP_SAVE_INTERVAL_MS
    // помечает dirty-персов на запись батчем.
    void UpdateSaveTimers();

    [[nodiscard]] static constexpr RepRank RankForStanding(int32 standing) noexcept {
        if (standing >= 42000)
            return RANK_EXALTED;
        if (standing >= 39000)
            return RANK_REVERED;
        if (standing >= 18000)
            return RANK_HONORED;
        if (standing >= 3000)
            return RANK_FRIENDLY;
        if (standing >= -3000)
            return RANK_NEUTRAL;
        if (standing >= -9000)
            return RANK_UNFRIENDLY;
        if (standing >= -36000)
            return RANK_HOSTILE;
        return RANK_HATED;
    }

    static bool CrossedRenegadeState(int32 repA, int32 repB) {
        return (repA < RENEGADE_THRESHOLD_A) && (repB < RENEGADE_THRESHOLD_B);
    }

private:
    ReputationMgr() = default;

    // Внутренний хелпер: чистая арифметика + cap + rate-множитель.
    int32 ApplyDelta(int32 cur, int32 delta, double rateMult) const noexcept;

    // Вызывает Lua-хук faction.renegade_gained (см. scripts/lua/faction/)
    // и авто-выдачу скрытого квеста q_1003 «Изгой».
    void OnBecomeRenegate(Player *player);
    void OnLeaveRenegate(Player *player); // только через GM/обряд искупления

    static constexpr uint32 REP_SAVE_INTERVAL_MS = 90000;
};

inline ReputationMgr &GetReputationMgr() {
    return ReputationMgr::Instance();
}

} // namespace DM
