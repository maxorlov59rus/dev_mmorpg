// CreatureAI.h — AI существ: смерть моба -> квестовый кредит + генерация лута.
#pragma once
#include <cstdint>
#include <vector>

namespace dm {

struct LootItem {
    uint32_t ItemId{0};
    uint32_t Count{1};

    friend bool operator==(const LootItem&, const LootItem&) = default;
};

struct LootRow { // строка creature_loot_template
    uint32_t Entry{0};   // creature entry
    uint32_t Item{0};
    double Chance{0.0};  // %
};

class Player;

class CreatureAI {
public:
    virtual ~CreatureAI() = default;

    // Хук смерти (вызывается из map-сервера при Unit::Kill):
    // 1) QuestLoader -> AdvanceQuestObjectiveOnKill(killer, m_entry)
    // 2) FillLoot() по lootid таблицы creature_loot_template
    void JustDied(Player* killer);

    // Реестр лута загружается при старте
    // (SELECT Entry,Item,Chance FROM creature_loot_template)
    static void RegisterLoot(const LootRow& row);
    static std::vector<LootItem> FillLoot(uint32_t creatureEntry, uint64_t rngSeed);

    void SetEntry(uint32_t entry) { m_entry = entry; }
    void SetLootId(uint32_t lootId) { m_lootId = lootId; }

private:
    uint32_t m_entry{0};
    uint32_t m_lootId{0};
};

} // namespace dm
