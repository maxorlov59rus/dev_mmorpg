// =============================================================================
// «Чёрная Материя» — dm-server / game/LFG/LFGMgr.h/cpp (артефакт 1, блок LFG)
// Менеджер очереди поиска группы (аналог TrinityCore LFGMgr, упрощённый до M1).
//
// Отличия от TRC по дизайну docs/02 §6: матчмейкинг НЕ на тайлере карт —
// отдельный тик в world update (1 Гц), чтобы не блокировать map-тикеты.
//
// Контракты:
//  * Вход в очередь разрешён при CanJoinLfg() (уровень диапазона + ачивка спеки).
//  * Роли: маска LfgRoleFlags (QuestSystem_Level10.h) — TANK/HEALER/DPS.
//  * После матча: инстанс создаётся через MapMgr::CreateInstance(mapId),
//    игрокам шлётся SMSG_LFG_PROPOSAL -> акцепт -> телепорт ticket'ом (docs/03).
//  * «Рупор призыва» (ITEM_LFG_HORN): consume-эффект предмета вызывает
//    CMSG_JOIN_LFG(dungeon=1) с дефолтной ролью игрока.
//
// Wire: proto/opcodes.defs v1.5, блок 0x2020..0x2027 (см. QuestSystem_Level10.h).
// =============================================================================
#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <vector>

namespace DM {
class Player;
}

namespace DM::Network {
class OpcodeDispatcher;
}

namespace DM::Game {

enum LfgQueueState : uint8_t {
    LFG_STATE_NONE = 0,
    LFG_STATE_QUEUED = 1,
    LFG_STATE_PROPOSAL = 2, // ждём акцепта
    LFG_STATE_IN_INSTANCE = 3,
};

struct LfgDungeonInfo {
    uint16_t Id = 0;
    uint8_t MinLevel = 10;
    uint8_t MaxLevel = 15;
    uint32_t MapId = 0;
    uint8_t RequiredRoles = 0; // маска минимально необходимых ролей (PvE: tank+healer)
    const char *NameRu = "";
};

class LFGMgr {
public:
    static LFGMgr &instance();

    bool loadFromDb(); // SELECT lfg_dungeons

    // === Очередь ===
    void JoinQueue(Player *player, uint16_t dungeonId, uint8_t roleMask);
    void LeaveQueue(Player *player);
    [[nodiscard]] bool IsQueued(uint64_t guid) const;
    [[nodiscard]] uint32_t EstimateWaitMs(uint16_t dungeonId) const; // EWMA по истории матчей

    // Тик матчмейкинга (1 Гц из WorldUpdate thread, НЕ из map tick).
    void Update();

    // Акцепт/отказ предложения (SMSG_LFG_PROPOSAL ответ клиента).
    void HandleProposalResponse(uint64_t guid, bool accept);

    // Полный сброс для теста/GM.
    void CancelAllForPlayer(uint64_t guid);

private:
    struct QueueEntry {
        uint64_t Guid = 0;
        uint16_t DungeonId = 0;
        uint8_t RoleMask = 0;
        uint64_t EnqueueTimeMs = 0;
    };

    struct Proposal {
        uint16_t DungeonId = 0;
        std::vector<uint64_t> Members; // guid'ы, ждут акцепта
        uint64_t CreatedMs = 0;
    };

    LFGMgr() = default;

    [[nodiscard]] const LfgDungeonInfo *FindDungeon(uint16_t id) const;
    void TryMatch(uint16_t dungeonId); // подбор кандидатов под RequiredRoles
    void FormGroup(const LfgDungeonInfo &dgn, std::vector<uint64_t> &&members);

    std::vector<LfgDungeonInfo> _dungeons;
    std::map<uint16_t, std::deque<QueueEntry>> _queues; // dungeonId -> FIFO
    std::map<uint64_t, uint16_t> _queuedAt;             // guid -> dungeonId (индекс)
    std::vector<Proposal> _proposals;                   // активные предложения
    std::map<uint64_t, uint64_t> _pendingAccept;        // guid -> proposalIdx(hash)
};

void RegisterLfgHandlers(Network::OpcodeDispatcher &disp, LFGMgr &mgr);

} // namespace DM::Game
