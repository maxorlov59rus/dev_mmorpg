// =============================================================================
// «Чёрная Материя» — dm-server / game/Quests/QuestGossipHandlers.cpp
// Обработчики: CMSG_GOSSIP_HELLO/SELECT, CMSG_QUEST_QUERY/ACCEPT/COMPLETE
// (opcodes 0x2000..0x2014, proto/opcodes.defs v3).
//
// Контракты:
//  * Скрытые ветки диалога НЕ отправляются клиенту (фильтр dm_conditions),
//    повторный guard на SELECT — защита от packet-replay.
//  * Дроп по заданиям: quest_drop_data (гарантированный квестовый предмет)
//    обрабатывается в LootMgr::RollLoot() после KillCredit; здесь только
//    прогресс kill/gather-goal'ей.
//  * Ренегат-инвариант и репутация — через ReputationMgr (game/Reputation).
// =============================================================================
#include "QuestGossipHandlers.h"

#include "Database/WorldDatabase.h"
#include "Game/Player/PlayerSession.h"
#include "Game/Quests/QuestMgr.h"
#include "Game/Reputation/ReputationMgr.h"
#include "Network/OpcodeDispatcher.h"
#include "Shared/Protocol/Opcodes.h"

#include <string>
#include <vector>

namespace DM::Game {

// ---------------------------------------------------------------------------
// Кэш gossip-данных (аналог TRC s_gossip_mgr, упрощённый до ядра)
// ---------------------------------------------------------------------------
// (GossipOptionRow — в QuestGossipHandlers.h; локально только кэш условий)
struct ConditionRow {
    uint32_t Id = 0;
    std::string Kind; // rep_rank_min | rep_rank_max | is_renegade | not_renegade | quest_active |
                      // faction_start
    int32_t Param1 = 0;
    int32_t Param2 = 0;
};

void GossipMgr::Load() {
    _options.clear();
    _conditions.clear();

    auto res =
        WorldDB().Query("SELECT gmo.MenuID, gmo.ID, gmo.OptionIcon, gmo.OptionText, "
                        "gmo.OptionBroadcastTextID, gmo.OptionType, gmo.Action, gmo.condition_id "
                        "FROM gossip_menu_option gmo ORDER BY gmo.MenuID, gmo.ID");
    while (auto row = res.NextRow()) {
        GossipOptionRow o;
        o.MenuId = row->GetUInt32(0);
        o.Id = static_cast<uint8_t>(row->GetUInt32(1));
        o.Icon = static_cast<uint8_t>(row->GetUInt32(2));
        o.Text = row->GetString(3);
        o.BroadcastTextId = row->GetUInt32(4);
        o.OptionType = static_cast<uint8_t>(row->GetUInt32(5));
        o.Action = row->GetUInt32(6);
        o.ConditionId = row->GetUInt32(7);
        _options[o.MenuId].push_back(std::move(o));
    }

    auto cres = WorldDB().Query("SELECT id, kind, param1, param2 FROM dm_conditions");
    while (auto row = cres.NextRow()) {
        ConditionRow c;
        c.Id = row->GetUInt32(0);
        c.Kind = row->GetString(1);
        c.Param1 = row->GetInt32(2);
        c.Param2 = row->GetInt32(3);
        _conditions[c.Id] = std::move(c);
    }

    sLog.Info("GossipMgr: загружено %zu меню, %zu условий", _options.size(), _conditions.size());
}

bool GossipMgr::ConditionSatisfied(const PlayerSession &player, uint32_t conditionId) const {
    if (conditionId == 0)
        return true;
    auto it = _conditions.find(conditionId);
    if (it == _conditions.end())
        return false; // неизвестное условие -> безопасно скрыть

    const ConditionRow &c = it->second;
    const ReputationMgr &rep = player.GetReputation();
    if (c.Kind == "rep_rank_min")
        return rep.GetRank(static_cast<FactionId>(c.Param1)) >= static_cast<RepRank>(c.Param2);
    if (c.Kind == "rep_rank_max")
        return rep.GetRank(static_cast<FactionId>(c.Param1)) <= static_cast<RepRank>(c.Param2);
    if (c.Kind == "is_renegade")
        return rep.IsRenegade(player);
    if (c.Kind == "not_renegade")
        return !rep.IsRenegade(player);
    if (c.Kind == "quest_active")
        return player.GetQuestMgr().IsCompleted(static_cast<uint32_t>(c.Param1));
    if (c.Kind == "faction_start")
        return player.GetStartFaction() == static_cast<FactionId>(c.Param1);
    return false;
}

std::vector<GossipOptionRow> GossipMgr::BuildVisibleOptions(uint32_t menuId,
                                                            const PlayerSession &player) const {
    std::vector<GossipOptionRow> visible;
    auto mit = _options.find(menuId);
    if (mit == _options.end())
        return visible;
    for (const GossipOptionRow &o : mit->second) {
        if (ConditionSatisfied(player, o.ConditionId))
            visible.push_back(o);
    }
    return visible;
}

const GossipOptionRow *GossipMgr::FindOption(uint32_t menuId, uint8_t optionId) const {
    auto mit = _options.find(menuId);
    if (mit == _options.end())
        return nullptr;
    for (const GossipOptionRow &o : mit->second)
        if (o.Id == optionId)
            return &o;
    return nullptr;
}

// ---------------------------------------------------------------------------
// Регистрация обработчиков (вызывается из WorldServer::SetupDispatchers)
// ---------------------------------------------------------------------------
void RegisterGossipHandlers(Network::OpcodeDispatcher &disp, GossipMgr &gossip, QuestMgr &quests) {
    disp.Register(Op::CMSG_GOSSIP_HELLO, [&gossip](PlayerSession &p, Network::Reader &r) {
        Guid npcGuid = r.ReadGuid();

        CreatureInstance *npc = p.Map().GetCreature(npcGuid);
        if (!npc || !npc->IsWithinInteractDistance(p)) // античит: too far -> ignore
            return;

        uint32_t menuId = npc->GetTemplate().GossipMenuId;
        if (menuId == 0)
            return;

        auto opts = gossip.BuildVisibleOptions(menuId, p);
        if (opts.empty())
            return;

        Network::Writer w(Op::SMSG_GOSSIP_SHOW);
        w.WriteGuid(npcGuid);
        w.WriteU32(menuId);
        w.WriteU32(npc->GetTemplate().GossipGreetingTextId); // broadcast_text заголовка
        w.WriteU8(static_cast<uint8_t>(opts.size()));
        for (const GossipOptionRow &o : opts) {
            w.WriteU8(o.Id);
            w.WriteU8(o.Icon);
            w.WriteU16(static_cast<uint16_t>(o.BroadcastTextId));
            w.WriteU8(quests.HasQuestForNpc(npc->GetEntry(), p) ? 1 : 0); // иконка !/?
        }
        p.SendPacket(w.Finish());
    });

    disp.Register(Op::CMSG_GOSSIP_SELECT, [&gossip, &quests](PlayerSession &p, Network::Reader &r) {
        Guid npcGuid = r.ReadGuid();
        uint8_t optionId = r.ReadU8();

        CreatureInstance *npc = p.Map().GetCreature(npcGuid);
        if (!npc || !npc->IsWithinInteractDistance(p))
            return;

        uint32_t menuId = npc->GetTemplate().GossipMenuId;
        const GossipOptionRow *opt = gossip.FindOption(menuId, optionId);
        // >>> REPLAY GUARD: клиент мог отправить опцию, которая ему не показывалась
        if (!opt || !gossip.ConditionSatisfied(p, opt->ConditionId)) {
            p.AntiCheat().Report(AcEvent::GossipReplay, menuId, optionId);
            return;
        }

        Network::Writer w(Op::SMSG_GOSSIP_REPLY);
        w.WriteGuid(npcGuid);
        w.WriteU32(opt->BroadcastTextId);
        w.WriteU8(MapActionToClient(opt->Action));
        w.WriteU32(quests.ResolveActionPayload(*npc, p, opt->Action));
        p.SendPacket(w.Finish());
    });

    // ---------------- QUESTS ----------------
    disp.Register(Op::CMSG_QUEST_QUERY, [&quests](PlayerSession &p, Network::Reader &) {
        quests.SendQuestList(p); // SMSG_QUEST_LIST: журнал + available зоны
    });

    disp.Register(Op::CMSG_QUEST_ACCEPT, [&quests](PlayerSession &p, Network::Reader &r) {
        uint32_t questId = r.ReadU32();
        quests.Accept(p, questId); // внутри: level-gate, faction-gate, chain-check
    });

    disp.Register(Op::CMSG_QUEST_COMPLETE, [&quests](PlayerSession &p, Network::Reader &r) {
        uint32_t questId = r.ReadU32();
        Guid npcGuid = r.ReadGuid();
        quests.Complete(p, questId, npcGuid);
        // Complete рассылает SMSG_QUEST_COMPLETE_RESULT + SMSG_REP_UPDATE
        // (RewRepValue в Q16 => ReputationMgr::ModifyReputation) и выдаёт
        // reward-предметы; RewItemId==5401 дополнительно ставит флаг
        // black_market_unlocked (SMSG_RENEGADE_STATE).
    });
}

uint8_t MapActionToClient(uint32_t serverAction) {
    // Таблица соответствия DB Action -> client action (см. opcodes.defs 0x2013)
    switch (serverAction) {
    case GOSSIP_ACTION_QUEST_MENU:
        return 2; // open_quest_log
    case GOSSIP_ACTION_VENDOR:
        return 1; // open_vendor
    case GOSSIP_ACTION_QUEST_TURN_IN:
        return 3;
    case GOSSIP_ACTION_REP_SUMMARY:
        return 4;
    case GOSSIP_ACTION_RENEGADE_BLACKMARKET:
        return 5; // black_market (клиент скрывает UI, если !black_market_unlocked)
    default:
        return 0; // close
    }
}

} // namespace DM::Game
