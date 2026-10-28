// quest_content_test.cpp — проверка ядра квестов 6-15 (цепочки, дроп, цели)
#include "../../dm-server/src/game/Quests/QuestLoader.h"
#include "../../dm-server/src/game/AI/CreatureAI.h"

#include <cassert>
#include <cstdio>

using namespace dm;

int main() {
    QuestLoader::Instance().LoadFromDatabase(nullptr);

    // 1) Реестр: 12 квестов
    assert(QuestLoader::Instance().Count() == 12);

    // 2) Цепочка 1: 1001->...->1005; цепочка 2: 1009->...->1012
    assert(QuestLoader::Instance().ChainNext(1001) == 1002);
    assert(QuestLoader::Instance().ChainNext(1004) == 1005);
    assert(QuestLoader::Instance().ChainNext(1005) == 1009);
    assert(QuestLoader::Instance().ChainNext(1012) == 0);

    // 3) Спец-флаги: 1004 = выбор спеки, 1005 = LFG-гейт
    assert(QuestLoader::Instance().Find(1004)->IsSpecChoice());
    assert(QuestLoader::Instance().Find(1005)->IsLfgGate());

    // 4) NPC-привязки
    assert(QuestLoader::Instance().QuestIdsForNpc(70011).size() == 4);
    assert(QuestLoader::Instance().QuestIdsForNpc(70015).size() == 3);

    // 5) Прогресс убийств: 5 слизней завершают цель 1001
    std::vector<PlayerQuestState> quests{{1001, {0}}};
    for (int i = 0; i < 4; ++i)
        assert(AdvanceQuestObjectiveOnKill(quests, 50001));
    assert(!QuestLoader::Instance().IsQuestObjectiveComplete(quests[0], 1001));
    assert(AdvanceQuestObjectiveOnKill(quests, 50001));
    assert(QuestLoader::Instance().IsQuestObjectiveComplete(quests[0], 1001));
    assert(!AdvanceQuestObjectiveOnKill(quests, 50001)); // кап достигнут

    // 6) Дроп по таблице creature_loot_template (слизни: Ихор 60%, Зелье 10%)
    CreatureAI::RegisterLoot(LootRow{50001, 11001, 60.0});
    CreatureAI::RegisterLoot(LootRow{50001, 10851, 10.0});
    int ichorHits = 0, potionHits = 0, rolls = 10000;
    for (int i = 0; i < rolls; ++i) {
        auto loot = CreatureAI::FillLoot(50001, uint64_t(i) * 2654435761ull + 1);
        for (const auto& l : loot) {
            if (l.ItemId == 11001) ++ichorHits;
            if (l.ItemId == 10851) ++potionHits;
        }
    }
    printf("loot stats: ichor=%.1f%% potion=%.1f%%\n", ichorHits * 100.0 / rolls, potionHits * 100.0 / rolls);
    assert(ichorHits > rolls * 0.55 && ichorHits < rolls * 0.65);
    assert(potionHits > rolls * 0.07 && potionHits < rolls * 0.13);

    // 7) Детерминизм: одинаковый seed -> одинаковый дроп
    assert(CreatureAI::FillLoot(50001, 42) == CreatureAI::FillLoot(50001, 42));

    std::puts("QUEST_CONTENT_TESTS_OK");
    return 0;
}
