// Player.h — заглушка
#pragma once
#include <cstdint>

namespace dm {
class QuestMgr {
public:
    void KilledCreature(uint32_t entry, int count) {}
};
class Player {
public:
    bool IsRenegate() const { return false; }
    QuestMgr *GetQuestMgr() { return nullptr; }
};
} // namespace dm
