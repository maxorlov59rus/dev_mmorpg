// Unit.h — заглушка
#pragma once
#include <cstdint>
#include "game/entities/Player.h"

namespace dm {
class Unit {
public:
    Player *GetPlayer() {
        return nullptr;
    }
    uint16_t GetFactionId() const {
        return 0;
    }
};
} // namespace dm
