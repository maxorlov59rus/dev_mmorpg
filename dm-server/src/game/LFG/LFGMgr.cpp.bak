// =============================================================================
// «Чёрная Материя» — dm-server / game/LFG/LFGMgr.cpp
// M1-реализация матчмейкинга: FIFO-очередь на данж, подбор по ролям, 4 места
// (tank+healer+2dps) или упрощённый PvE-пакет из 3; timeout предложения 60 c.
// TODO(KERNEL)-заглушки — тот же контракт, что в PlayerClassMgr/QuestSystem_Level10.
// =============================================================================
#include "LFGMgr.h"

#include "Database/WorldDatabase.h"
#include "Game/Player/PlayerSession.h"
#include "Game/Quests/QuestSystem_Level10.h"
#include "Network/OpcodeDispatcher.h"
#include "Shared/Protocol/Opcodes.h"

#include <algorithm>
#include <chrono>
#include <cstdio>

namespace DM::Game {

namespace {
uint64_t NowMs() {
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}
constexpr uint64_t kProposalTimeoutMs = 60'000;
constexpr size_t kGroupSize = 4; // tank + healer + 2 dps (обучающий инстанс)
} // namespace

LFGMgr &LFGMgr::instance() {
    static LFGMgr s;
    return s;
}

bool LFGMgr::loadFromDb() {
    _dungeons.clear();
    auto res = WorldDB().Query(
        "SELECT ID, MinLevel, MaxLevel, MapID, RequiredRolesMask, NameRu FROM lfg_dungeons");
    while (auto row = res.NextRow()) {
        LfgDungeonInfo d;
        d.Id = static_cast<uint16_t>(row->GetUInt32(0));
        d.MinLevel = static_cast<uint8_t>(row->GetUInt32(1));
        d.MaxLevel = static_cast<uint8_t>(row->GetUInt32(2));
        d.MapId = row->GetUInt32(3);
        d.RequiredRoles = static_cast<uint8_t>(row->GetUInt32(4));
        // NameRu — string_view живёт в строке БД только до reload; копируем в статик-пул M2.
        d.NameRu = ""; // TODO(M2): StringPool
        _dungeons.push_back(d);
    }
    std::printf("[LFGMgr] загружено %d данжей\n", static_cast<int>(_dungeons.size()));
    return true;
}

const LfgDungeonInfo *LFGMgr::FindDungeon(uint16_t id) const {
    for (const auto &d : _dungeons)
        if (d.Id == id)
            return &d;
    return nullptr;
}

void LFGMgr::JoinQueue(Player *player, uint16_t dungeonId, uint8_t roleMask) {
    if (!player || roleMask == LFG_ROLE_NONE)
        return;
    // TODO(KERNEL): const uint64 guid = player->GetGUID();
    const uint64_t guid = 0; // placeholder до Entities/Player.h

    if (_queuedAt.count(guid))
        return; // уже в очереди (дедупликация packet-spoof)

    QueueEntry e;
    e.Guid = guid;
    e.DungeonId = dungeonId;
    e.RoleMask = roleMask;
    e.EnqueueTimeMs = NowMs();
    _queues[dungeonId].push_back(e);
    _queuedAt[guid] = dungeonId;

    std::printf("[LFG] guid=%llu queued dungeon=%u roles=0x%02X\n",
                static_cast<unsigned long long>(guid), dungeonId, roleMask);
}

void LFGMgr::LeaveQueue(Player *player) {
    // TODO(KERNEL): const uint64 guid = player->GetGUID();
    const uint64_t guid = 0;
    CancelAllForPlayer(guid);
}

bool LFGMgr::IsQueued(uint64_t guid) const {
    return _queuedAt.count(guid) != 0;
}

void LFGMgr::CancelAllForPlayer(uint64_t guid) {
    auto it = _queuedAt.find(guid);
    if (it == _queuedAt.end())
        return;
    auto &dq = _queues[it->second];
    std::erase_if(dq, [&](const QueueEntry &e) { return e.Guid == guid; });
    _queuedAt.erase(it);
    // отзыв из pending-предложения
    auto pa = _pendingAccept.find(guid);
    if (pa != _pendingAccept.end()) {
        _pendingAccept.erase(pa);
        // TODO: decrement proposal accept-count -> если все отказались — расформировать.
    }
}

uint32_t LFGMgr::EstimateWaitMs(uint16_t dungeonId) const {
    // EWMA по последним матчам (M1: эвристика — размер очереди × тик).
    auto q = _queues.find(dungeonId);
    if (q == _queues.end())
        return 30'000;
    const uint32_t need = kGroupSize;
    if (q->second.size() >= need)
        return 5'000; // почти собрано
    return static_cast<uint32_t>(15'000 + (need - q->second.size()) * 20'000);
}

// -----------------------------------------------------------------------------
// Тик матчмейкинга (1 Гц)
// -----------------------------------------------------------------------------
void LFGMgr::Update() {
    // 1) Протухшие предложения (timeout 60 c) — расформировать, вернуть в очередь.
    const uint64_t now = NowMs();
    for (auto pit = _proposals.begin(); pit != _proposals.end();) {
        if (now - pit->CreatedMs > kProposalTimeoutMs) {
            for (uint64_t g : pit->Members)
                _pendingAccept.erase(g);
            std::printf("[LFG] proposal timeout dungeon=%u members=%zu\n", pit->DungeonId,
                        pit->Members.size());
            pit = _proposals.erase(pit);
        } else {
            ++pit;
        }
    }

    // 2) Попытка собрать группу по каждому данжу.
    for (auto &[dungeonId, dq] : _queues)
        TryMatch(dungeonId);
}

void LFGMgr::TryMatch(uint16_t dungeonId) {
    const LfgDungeonInfo *dgn = FindDungeon(dungeonId);
    auto qIt = _queues.find(dungeonId);
    if (!dgn || qIt == _queues.end() || qIt->second.size() < kGroupSize)
        return;

    auto &dq = qIt->second;
    // Жадный подбор: сначала tank, healer, потом DPS (FIFO внутри роли).
    std::vector<uint64_t> picked;
    std::vector<size_t> pickedIdx;
    uint8_t have = LFG_ROLE_NONE;
    for (size_t i = 0; i < dq.size() && picked.size() < kGroupSize; ++i) {
        const QueueEntry &e = dq[i];
        if (_pendingAccept.count(e.Guid))
            continue; // уже в чужом предложении
        uint8_t take = e.RoleMask & static_cast<uint8_t>(~have);
        if (take == 0 && picked.size() + 2 < kGroupSize)
            take = e.RoleMask & LFG_ROLE_DPS; // сверхкомплект DPS разрешён, если не хватает ролей
        if (take == 0)
            continue;
        have |= take;
        picked.push_back(e.Guid);
        pickedIdx.push_back(i);
    }

    const bool rolesOk = (have & dgn->RequiredRoles) == dgn->RequiredRoles;
    if (picked.size() < kGroupSize || !rolesOk)
        return;

    FormGroup(*dgn, std::move(picked));
    // Удаляем участников из очереди (индексы убывающем порядке — не сдвигаем).
    std::sort(pickedIdx.rbegin(), pickedIdx.rend());
    for (size_t idx : pickedIdx)
        dq.erase(dq.begin() + static_cast<long>(idx));
}

void LFGMgr::FormGroup(const LfgDungeonInfo &dgn, std::vector<uint64_t> &&members) {
    Proposal prop;
    prop.DungeonId = dgn.Id;
    prop.Members = members;
    prop.CreatedMs = NowMs();
    size_t propIdx = _proposals.size();
    _proposals.push_back(std::move(prop));

    for (uint64_t g : _proposals[propIdx].Members) {
        _queuedAt.erase(g);
        _pendingAccept[g] = propIdx;

        // SMSG_LFG_PROPOSAL { u16 dungeon_id u64 group_leader_guid? } — клиент
        // показывает модалку «Группа собрана» с акцептом (LFGQueueWidget).
        PacketBuilder p(SMSG_LFG_PROPOSAL);
        p.WriteU16(dgn.Id);
        p.WriteU32(dgn.MapId);
        p.WriteU64(/*leader=*/g); // M1: leader = первый по FIFO
        // TODO(KERNEL): SendToGuid(g, p.Build());
    }
    std::printf("[LFG] proposal sent dungeon=%u map=%u members=%zu\n", dgn.Id, dgn.MapId,
                _proposals[propIdx].Members.size());
}

void LFGMgr::HandleProposalResponse(uint64_t guid, bool accept) {
    auto pa = _pendingAccept.find(guid);
    if (pa == _pendingAccept.end())
        return;
    size_t idx = static_cast<size_t>(pa->second);
    if (idx >= _proposals.size()) {
        _pendingAccept.erase(pa);
        return;
    }
    Proposal &prop = _proposals[idx];

    if (!accept) {
        // Отказ: вся группа расходится по очереди (TRC-поведение — пенальти-кулдаун M2).
        _pendingAccept.erase(pa);
        for (uint64_t g : prop.Members) {
            if (g == guid)
                continue;
            _pendingAccept.erase(g);
            // вернуть в очередь с исходным roleMask — M2: хранить снапшот.
        }
        std::printf("[LFG] proposal declined by guid=%llu dungeon=%u\n",
                    static_cast<unsigned long long>(guid), prop.DungeonId);
        _proposals.erase(_proposals.begin() + static_cast<long>(idx));
        return;
    }

    _pendingAccept.erase(pa);
    // Все акцепнули? -> создаём инстанс и телепортим.
    bool allAccepted = true;
    for (uint64_t g : prop.Members)
        if (_pendingAccept.count(g))
            allAccepted = false;
    if (!allAccepted)
        return;

    const LfgDungeonInfo *dgn = FindDungeon(prop.DungeonId);
    if (dgn) {
        // TODO(KERNEL): uint32 instanceId = MapMgr::CreateInstance(dgn->MapId, /*difficulty*/0);
        //   for (g : prop.Members) TeleportMgr::SendTicket(g, dgn->MapId, instanceId);
        //   SpecQuestMgr: прогресс квеста 1105 при входе в инстанс (area trigger).
        std::printf("[LFG] group formed -> instance map=%u members=%zu\n", dgn->MapId,
                    prop.Members.size());
    }
    _proposals.erase(_proposals.begin() + static_cast<long>(idx));
}

// -----------------------------------------------------------------------------
// Сетевые обработчики LFG-блока (0x2020..0x2022 + proposal response)
// -----------------------------------------------------------------------------
void RegisterLfgHandlers(Network::OpcodeDispatcher &disp, LFGMgr &mgr) {
    disp.Register(CMSG_JOIN_LFG_RESPONSE_UNUSED, [](PlayerSession &, const BinaryPacketReader &) {
        // Зарезервировано: фактический вход обрабатывает RegisterSpecQuestHandlers
        // (гейт CanJoinLfg живёт рядом с ChooseSpec, чтобы один guard кормил оба).
    });

    disp.Register(CMSG_LFG_LEAVE, [&mgr](PlayerSession &ps, const BinaryPacketReader &) {
        mgr.LeaveQueue(ps.GetPlayer());
    });

    disp.Register(CMSG_LFG_PROPOSAL_ACCEPT, [&mgr](PlayerSession &ps, const BinaryPacketReader &r) {
        uint8 acc = 0;
        if (!r.ReadU8(acc))
            return;
        // TODO(KERNEL): mgr.HandleProposalResponse(ps.GetPlayer()->GetGUID(), acc != 0);
        (void)ps;
    });
}

} // namespace DM::Game
