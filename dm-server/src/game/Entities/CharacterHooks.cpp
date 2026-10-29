// CharacterHooks.cpp — OnCreatureKillForQuest: прогресс целей KillCredit.
#include "../Quests/QuestLoader.h"

#include <algorithm>

// TODO(KERNEL): включить в класс Character (dm-server/src/game/Entities/Character.h):
//   void OnCreatureKillForQuest(uint32_t creatureEntry);
//   std::vector<PlayerQuestState> m_activeQuests;
namespace dm {

bool AdvanceQuestObjectiveOnKill(std::vector<PlayerQuestState> &activeQuests,
                                 uint32_t creatureEntry) {
    bool advanced = false;
    const QuestLoader &loader = QuestLoader::Instance();
    for (auto &state : activeQuests) {
        const QuestData *q = loader.Find(state.QuestId);
        if (!q)
            continue;
        for (const auto &obj : q->Objectives) {
            if (obj.Type != QuestObjectiveType::KillCredit || obj.TargetEntry != creatureEntry)
                continue;
            if (obj.Index >= state.CountsPerObjective.size())
                state.CountsPerObjective.resize(obj.Index + 1, 0);
            uint32_t &c = state.CountsPerObjective[obj.Index];
            if (c < obj.RequiredCount) {
                ++c;
                advanced = true;
            }
        }
    }
    return advanced;
}

} // namespace dm
