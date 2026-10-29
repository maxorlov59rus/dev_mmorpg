// SmartAI.h
#pragma once
#include <cstdint>

namespace dm {
class Creature;
class Player;
class Unit;
class WorldLocation;

class SmartAI {
public:
    static bool CanAttackRenegade(const Creature *creature, const Player *player);
    static void OnPlayerEnteringRange(Creature *me, Player *player, const WorldLocation &loc);
    static void OnEnterCombat(Creature *me, Unit *victim);
    static void OnCombatStop(Creature *me);
    static void OnHealthPercent(Creature *me, uint8_t pctHp);
    static void OnJustDied(Creature *me);
    static void UpdateHostileTaunts(Creature *me, uint32_t diffMs);
    static void OnKillCredit(Player *killer, void *killed, const void *params);
};
} // namespace dm
