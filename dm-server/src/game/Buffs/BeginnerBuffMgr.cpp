#include "game/Buffs/BeginnerBuffMgr.h"
#include <unordered_map>
#include <string>

namespace dm {

static bool s_enabled = true;
static uint8_t s_levelCap = 10;
static bool s_autoApply = true;
static bool s_showUI = true;
static bool s_removeOnLevel11 = true;

static BeginnerBuffStats s_buffStats = {
    1.5f,  // xpMultiplier
    1.25f, // dropMultiplier
    1.25f, // goldMultiplier
    1.1f,  // movementSpeed
    0.1f   // damageReduction
};

BeginnerBuffMgr &BeginnerBuffMgr::Instance() {
    static BeginnerBuffMgr inst;
    return inst;
}

bool BeginnerBuffMgr::IsBeginnerBuffActive(uint32_t playerGuid, uint8_t playerLevel) const {
    if (!s_enabled) return false;
    return playerLevel >= 1 && playerLevel <= s_levelCap;
}

bool BeginnerBuffMgr::ShouldApplyBuff(uint8_t playerLevel) const {
    if (!s_enabled || !s_autoApply) return false;
    return playerLevel >= 1 && playerLevel <= s_levelCap;
}

bool BeginnerBuffMgr::ShouldRemoveBuff(uint8_t playerLevel) const {
    if (!s_removeOnLevel11) return false;
    return playerLevel >= s_levelCap + 1;
}

BeginnerBuffStats BeginnerBuffMgr::GetBuffStats() const {
    return s_buffStats;
}

float BeginnerBuffMgr::GetXPMultiplier(uint8_t playerLevel) const {
    if (!IsBeginnerBuffActive(0, playerLevel)) return 1.0f;
    return s_buffStats.xpMultiplier;
}

float BeginnerBuffMgr::GetDropMultiplier(uint8_t playerLevel) const {
    if (!IsBeginnerBuffActive(0, playerLevel)) return 1.0f;
    return s_buffStats.dropMultiplier;
}

float BeginnerBuffMgr::GetGoldMultiplier(uint8_t playerLevel) const {
    if (!IsBeginnerBuffActive(0, playerLevel)) return 1.0f;
    return s_buffStats.goldMultiplier;
}

float BeginnerBuffMgr::GetMovementSpeedMultiplier(uint8_t playerLevel) const {
    if (!IsBeginnerBuffActive(0, playerLevel)) return 1.0f;
    return s_buffStats.movementSpeed;
}

float BeginnerBuffMgr::GetDamageReduction(uint8_t playerLevel) const {
    if (!IsBeginnerBuffActive(0, playerLevel)) return 0.0f;
    return s_buffStats.damageReduction;
}

void BeginnerBuffMgr::ApplyBuff(uint32_t playerGuid, uint8_t playerLevel) {
    if (!ShouldApplyBuff(playerLevel)) return;
    // TODO: Apply buff to player
    // TODO: Log to database
}

void BeginnerBuffMgr::RemoveBuff(uint32_t playerGuid, uint8_t playerLevel) {
    if (!ShouldRemoveBuff(playerLevel)) return;
    // TODO: Remove buff from player
    // TODO: Log to database
}

bool BeginnerBuffMgr::IsEnabled() const { return s_enabled; }
uint8_t BeginnerBuffMgr::GetLevelCap() const { return s_levelCap; }
bool BeginnerBuffMgr::IsAutoApply() const { return s_autoApply; }
bool BeginnerBuffMgr::ShowInUI() const { return s_showUI; }

void BeginnerBuffMgr::LoadConfig() {
    // TODO: Load from database
}

} // namespace dm