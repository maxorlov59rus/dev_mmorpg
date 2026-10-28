// CreatureAI.cpp — JustDied: квестовый кредит игрока + детерминированный дроп
#include "CreatureAI.h"
#include "../Quests/QuestLoader.h"

#include <unordered_map>

// TODO(KERNEL): реальный Player API (dm-server/src/game/Entities/Player.h):
//   GetGuid(), GetSession()->Add Loot to container, SendPacket(SMSG_QUEST_UPDATE...).

namespace dm {
namespace {
std::unordered_map<uint32_t, std::vector<LootRow>>& LootRegistry() {
    static std::unordered_map<uint32_t, std::vector<LootRow>> reg;
    return reg;
}

// xorshift64* — детерминированный RNG для unit-тестов дропа
uint64_t NextRand(uint64_t& s) {
    s ^= s >> 12; s ^= s << 25; s ^= s >> 27;
    return s * 0x2545F4914F6CDD1Dull;
}
} // namespace

void CreatureAI::RegisterLoot(const LootRow& row) {
    LootRegistry()[row.Entry].push_back(row);
}

std::vector<LootItem> CreatureAI::FillLoot(uint32_t creatureEntry, uint64_t rngSeed) {
    std::vector<LootItem> out;
    auto it = LootRegistry().find(creatureEntry);
    if (it == LootRegistry().end())
        return out;

    uint64_t seed = rngSeed ? rngSeed : 0x9E3779B97F4A7C15ull;
    for (const LootRow& r : it->second) {
        const double roll = double(NextRand(seed) % 10000u) / 100.0; // 0..100.00
        if (roll < r.Chance)
            out.push_back(LootItem{r.Item, 1});
    }
    return out;
}

void CreatureAI::JustDied(Player* /*killer*/) {
    // 1) Квестовый кредит: хук Character::OnCreatureKillForQuest (см. CharacterHooks.cpp)
    //    questId-цели ищет QuestLoader по TargetEntry == m_entry.
    // 2) Лут: FillLoot(m_lootId ? m_lootId : m_entry, seed от GUID killer+tick)
    // TODO(KERNEL): передать killer->GetGUID() как соль и создать LootContainer.
}

} // namespace dm
