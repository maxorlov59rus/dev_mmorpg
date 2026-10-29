// Creature.h — заглушка
#pragma once
#include <cstdint>

namespace dm {
class Creature {
public:
    void SetEntry(uint32_t entry) {}
    void SetLootId(uint32_t lootId) {}
    uint32_t GetEntry() const {
        return 0;
    }
    uint32_t GetFactionId() const {
        return 0;
    }
    uint8_t GetAiTimer(int) const {
        return 0;
    }
    void SetAiTimer(int, int) {}
    void MonsterTextEmote(uint32_t, Player *, bool) {}
    Player *GetVictim() {
        return nullptr;
    }
};
} // namespace dm
