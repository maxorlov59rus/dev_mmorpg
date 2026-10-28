// =============================================================================
// «Чёрная Материя» — dm-server / game/Quests/QuestGossipHandlers.h
// GossipMgr + регистрация квестовых/gossip-обработчиков (docs/02 §Scripts,
// sql/upgrades/StarterContent_L1_5.sql).
// =============================================================================
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace DM::Network {
class OpcodeDispatcher;
}

namespace DM::Game {
class PlayerSession;
class QuestMgr;

struct GossipOptionRow {
    uint32_t MenuId = 0;
    uint8_t Id = 0;
    uint8_t Icon = 0;
    std::string Text;
    uint32_t BroadcastTextId = 0;
    uint8_t OptionType = 0;
    uint32_t Action = 0;
    uint32_t ConditionId = 0; // 0 = всегда видна
};

// Действия gossip_menu_option.Action (БД) — расширенный набор TRC:
enum GossipAction : uint32_t {
    GOSSIP_ACTION_TALK = 0,
    GOSSIP_ACTION_VENDOR = 1,
    GOSSIP_ACTION_QUEST_MENU = 2,
    GOSSIP_ACTION_REP_SUMMARY = 3,
    GOSSIP_ACTION_BANK = 4,
    GOSSIP_ACTION_RENEGADE_BLACKMARKET = 5, // только изгои (condition is_renegade)
    GOSSIP_ACTION_TRAINER = 6,
    GOSSIP_ACTION_HEALER = 7,
    GOSSIP_ACTION_BANKER = 8,
    GOSSIP_ACTION_MAILBOX = 9, // нейтральный Вестовой Порога — доступен всем, включая изгоев
};

class GossipMgr {
public:
    void Load(); // SELECT gossip_menu_option + dm_conditions -> кэш

    [[nodiscard]] std::vector<GossipOptionRow>
    BuildVisibleOptions(uint32_t menuId, const PlayerSession &player) const;
    [[nodiscard]] bool ConditionSatisfied(const PlayerSession &player, uint32_t conditionId) const;
    [[nodiscard]] const GossipOptionRow *FindOption(uint32_t menuId, uint8_t optionId) const;

private:
    struct ConditionRow;
    std::map<uint32_t, std::vector<GossipOptionRow>> _options; // MenuID -> опции (по ID)
    std::unordered_map<uint32_t, ConditionRow> _conditions;    // condition_id -> предикат
};

void RegisterGossipHandlers(Network::OpcodeDispatcher &disp, GossipMgr &gossip, QuestMgr &quests);
uint8_t MapActionToClient(uint32_t serverAction);

} // namespace DM::Game
