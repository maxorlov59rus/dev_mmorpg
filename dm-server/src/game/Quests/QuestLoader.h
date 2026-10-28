// QuestLoader.h — загрузка квестов 6-15 в ядро (контракт: sql/upgrades/Insert_Quests_6_15.sql)
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace dm {

enum class QuestObjectiveType : uint8_t {
    KillCredit = 1, // цель убита (TargetEntry = creature entry)
    ItemGather = 2, // собрать предмет/ресурс GO (TargetEntry = item/GO entry)
    ExploreArea = 3,// посетить зону (TargetEntry = areatable.id)
    TalkNpc = 4     // поговорить с NPC (TargetEntry = npc entry)
};

struct QuestObjective {
    uint32_t Id{0};
    QuestObjectiveType Type{};
    uint8_t Index{0};
    uint32_t TargetEntry{0};
    uint32_t RequiredCount{1};
    std::string Text;
};

struct QuestData {
    uint32_t Id{0};
    std::string Title;
    uint8_t MinLevel{1};
    uint8_t MaxLevel{95};
    uint32_t ZoneId{0};
    uint32_t NextQuestId{0};   // цепочка
    uint32_t PrevQuestId{0};
    uint8_t QuestType{1};      // 1=PvE, 2=групповой/инстанс
    uint8_t SpecialFlags{0};   // bit1=spec-choice(1004), bit2=LFG-gate(1005)
    int32_t RewardRepFaction{0};
    int32_t RewardRepAmount{0};
    uint32_t RewardMoney{0};
    std::vector<QuestObjective> Objectives;

    bool IsSpecChoice() const { return (SpecialFlags & 0x1) != 0; }
    bool IsLfgGate() const { return (SpecialFlags & 0x2) != 0; }
};

// Singleton-реестр квестов. Загружается из БД при старте worldserver.
class QuestLoader {
public:
    static QuestLoader& Instance();

    // SELECT-контракт: quest_template + quest_objective (см. Insert_Quests_6_15.sql)
    bool LoadFromDatabase(class SqlSession* session);

    const QuestData* Find(uint32_t questId) const;
    const std::vector<uint32_t>& QuestIdsForNpc(uint32_t npcEntry) const;
    uint32_t ChainNext(uint32_t questId) const;

    // Проверка цели квеста после убийства моба (хук из CreatureAI::JustDied)
    bool IsQuestObjectiveComplete(const struct PlayerQuestState& state, uint32_t questId) const;

    size_t Count() const { return m_quests.size(); }

private:
    QuestLoader() = default;
    std::unordered_map<uint32_t, QuestData> m_quests;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_npcToQuests; // npc -> quest ids
};

// Прогресс игрока по целям одного квеста (хранит Character::m_activeQuests)
struct PlayerQuestState {
    uint32_t QuestId{0};
    std::vector<uint32_t> CountsPerObjective; // индекс = QuestObjective::Index
};

// Реализация: CharacterHooks.cpp (хук OnCreatureKillForQuest из AI смерти моба).
bool AdvanceQuestObjectiveOnKill(std::vector<PlayerQuestState>& activeQuests,
                                 uint32_t creatureEntry);

} // namespace dm
