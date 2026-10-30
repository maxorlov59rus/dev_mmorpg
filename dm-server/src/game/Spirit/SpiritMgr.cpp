#include "game/Spirit/SpiritMgr.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>

namespace dm {

static std::unordered_map<uint32_t, std::vector<Spirit>> s_spirits;
static std::unordered_map<uint32_t, uint64_t> s_activeSpirit;
static std::unordered_map<uint8_t, std::vector<SpiritSkill>> s_skillsByClass;
static uint64_t s_nextSpiritId = 1;

SpiritMgr &SpiritMgr::Instance() {
    static SpiritMgr inst;
    return inst;
}

Spirit SpiritMgr::RollSpirit(uint32_t playerGuid) {
    Spirit spirit;
    spirit.spiritId = s_nextSpiritId++;
    spirit.playerGuid = playerGuid;
    spirit.level = 1;
    spirit.experience = 0;
    spirit.isActive = false;
    spirit.rollsUsedToday = 0;
    spirit.lastRollDate = 0;
    spirit.createdAt = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    float roll = dis(gen);
    if (roll < 0.03f) {
        spirit.quality = SpiritQuality::Legendary;
    } else if (roll < 0.15f) {
        spirit.quality = SpiritQuality::Epic;
    } else if (roll < 0.40f) {
        spirit.quality = SpiritQuality::Rare;
    } else {
        spirit.quality = SpiritQuality::Common;
    }

    if (dis(gen) < 0.10f && spirit.quality != SpiritQuality::Legendary) {
        spirit.quality = static_cast<SpiritQuality>(static_cast<uint8_t>(spirit.quality) + 1);
    }

    std::uniform_int_distribution<int> classDis(1, 4);
    spirit.spiritClass = static_cast<SpiritClass>(classDis(gen));

    uint32_t slotCount = GetSkillSlotCount(spirit.quality);
    auto skills = GetSkillsForClass(spirit.spiritClass);
    std::shuffle(skills.begin(), skills.end(), gen);
    for (uint32_t i = 0; i < slotCount && i < skills.size(); i++) {
        spirit.skills.push_back(skills[i].skillId);
        spirit.skillLevels[skills[i].skillId] = 1;
    }

    return spirit;
}

bool SpiritMgr::CanRoll(uint32_t playerGuid) const {
    uint32_t used = 0;
    auto it = s_spirits.find(playerGuid);
    if (it != s_spirits.end()) {
        for (const auto &s : it->second) {
            used += s.rollsUsedToday;
        }
    }
    return used < GetDailyRolls();
}

uint32_t SpiritMgr::GetRollsRemaining(uint32_t playerGuid) const {
    uint32_t used = 0;
    auto it = s_spirits.find(playerGuid);
    if (it != s_spirits.end()) {
        for (const auto &s : it->second) {
            used += s.rollsUsedToday;
        }
    }
    uint32_t daily = GetDailyRolls();
    if (used >= daily) return 0;
    return daily - used;
}

void SpiritMgr::ResetDailyRolls(uint32_t playerGuid) {
    auto it = s_spirits.find(playerGuid);
    if (it != s_spirits.end()) {
        for (auto &s : it->second) {
            s.rollsUsedToday = 0;
        }
    }
}

bool SpiritMgr::AddSpirit(uint32_t playerGuid, const Spirit &spirit) {
    if (GetSpiritCount(playerGuid) >= GetMaxSpirits()) return false;
    s_spirits[playerGuid].push_back(spirit);
    return true;
}

bool SpiritMgr::RemoveSpirit(uint32_t playerGuid, uint64_t spiritId) {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return false;
    for (auto sit = it->second.begin(); sit != it->second.end(); ++sit) {
        if (sit->spiritId == spiritId) {
            it->second.erase(sit);
            auto ait = s_activeSpirit.find(playerGuid);
            if (ait != s_activeSpirit.end() && ait->second == spiritId) {
                s_activeSpirit.erase(ait);
            }
            return true;
        }
    }
    return false;
}

bool SpiritMgr::SetActiveSpirit(uint32_t playerGuid, uint64_t spiritId) {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return false;
    for (auto &s : it->second) {
        if (s.spiritId == spiritId) {
            for (auto &s2 : it->second) s2.isActive = false;
            s.isActive = true;
            s_activeSpirit[playerGuid] = spiritId;
            return true;
        }
    }
    return false;
}

Spirit *SpiritMgr::GetActiveSpirit(uint32_t playerGuid) {
    auto ait = s_activeSpirit.find(playerGuid);
    if (ait == s_activeSpirit.end()) return nullptr;
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return nullptr;
    for (auto &s : it->second) {
        if (s.spiritId == ait->second) return &s;
    }
    return nullptr;
}

std::vector<Spirit> SpiritMgr::GetSpirits(uint32_t playerGuid) const {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return {};
    return it->second;
}

uint32_t SpiritMgr::GetSpiritCount(uint32_t playerGuid) const {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return 0;
    return (uint32_t)it->second.size();
}

void SpiritMgr::SyncLevelWithPlayer(uint32_t playerGuid, uint8_t playerLevel) {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return;
    for (auto &s : it->second) {
        if (s.level < playerLevel) {
            s.level = playerLevel;
            s.experience = 0;
            // Auto-upgrade skills to match new level
            for (uint32_t skillId : s.skills) {
                s.skillLevels[skillId] = s.level;
            }
        } else if (s.level > playerLevel) {
            s.level = playerLevel;
            s.experience = 0;
            for (uint32_t skillId : s.skills) {
                s.skillLevels[skillId] = s.level;
            }
        }
    }
}

bool SpiritMgr::IsSpiritLevelSynced(uint32_t playerGuid, uint64_t spiritId, uint8_t playerLevel) const {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return false;
    for (const auto &s : it->second) {
        if (s.spiritId == spiritId) {
            return s.level == playerLevel;
        }
    }
    return false;
}


bool SpiritMgr::AddExperience(uint32_t playerGuid, uint64_t spiritId, uint64_t exp) {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return false;
    for (auto &s : it->second) {
        if (s.spiritId == spiritId) {
            s.experience += exp;
            uint64_t expNeeded = s.level * 100;
            if (s.experience >= expNeeded) {
                LevelUp(playerGuid, spiritId);
            }
            return true;
        }
    }
    return false;
}

bool SpiritMgr::LevelUp(uint32_t playerGuid, uint64_t spiritId) {
    auto it = s_spirits.find(playerGuid);
    if (it == s_spirits.end()) return false;
    for (auto &s : it->second) {
        if (s.spiritId == spiritId) {
            uint64_t expNeeded = s.level * 100;
            if (s.experience >= expNeeded) {
                s.experience -= expNeeded;
                s.level++;
                // Auto-upgrade skills: each skill level increases by 1 per spirit level
                for (uint32_t skillId : s.skills) {
                    s.skillLevels[skillId] = s.level;
                }
                return true;
            }
        }
    }
    return false;
}

std::vector<SpiritSkill> SpiritMgr::GetSkillsForClass(SpiritClass cls) const {
    auto it = s_skillsByClass.find(static_cast<uint8_t>(cls));
    if (it == s_skillsByClass.end()) return {};
    return it->second;
}

uint32_t SpiritMgr::GetSkillSlotCount(SpiritQuality quality) const {
    switch (quality) {
        case SpiritQuality::Common: return 1;
        case SpiritQuality::Rare: return 2;
        case SpiritQuality::Epic: return 3;
        case SpiritQuality::Legendary: return 4;
    }
    return 1;
}

void SpiritMgr::LoadSpiritSkills() {
    SpiritSkill s1; s1.skillId = 1; s1.classId = 1; s1.nameRu = "Удар щитом"; s1.nameEn = "Shield Bash"; s1.damageType = "physical"; s1.baseDamage = 50; s1.cooldownMs = 8000; s1.manaCost = 20;
    SpiritSkill s2; s2.skillId = 2; s2.classId = 1; s2.nameRu = "Провокация"; s2.nameEn = "Taunt"; s2.damageType = "physical"; s2.baseDamage = 0; s2.cooldownMs = 10000; s2.manaCost = 15;
    SpiritSkill s3; s3.skillId = 3; s3.classId = 1; s3.nameRu = "Каменная кожа"; s3.nameEn = "Stone Skin"; s3.damageType = "physical"; s3.baseDamage = 0; s3.cooldownMs = 15000; s3.manaCost = 30;
    s_skillsByClass[1] = {s1, s2, s3};

    SpiritSkill s4; s4.skillId = 4; s4.classId = 2; s4.nameRu = "Исцеление"; s4.nameEn = "Heal"; s4.damageType = "holy"; s4.baseDamage = 100; s4.cooldownMs = 6000; s4.manaCost = 25;
    SpiritSkill s5; s5.skillId = 5; s5.classId = 2; s5.nameRu = "Благословение"; s5.nameEn = "Blessing"; s5.damageType = "holy"; s5.baseDamage = 0; s5.cooldownMs = 12000; s5.manaCost = 20;
    SpiritSkill s6; s6.skillId = 6; s6.classId = 2; s6.nameRu = "Очищение"; s6.nameEn = "Purify"; s6.damageType = "holy"; s6.baseDamage = 0; s6.cooldownMs = 10000; s6.manaCost = 15;
    s_skillsByClass[2] = {s4, s5, s6};

    SpiritSkill s7; s7.skillId = 7; s7.classId = 3; s7.nameRu = "Огненный шар"; s7.nameEn = "Fireball"; s7.damageType = "fire"; s7.baseDamage = 80; s7.cooldownMs = 5000; s7.manaCost = 20;
    SpiritSkill s8; s8.skillId = 8; s8.classId = 3; s8.nameRu = "Ледяная стрела"; s8.nameEn = "Frost Bolt"; s8.damageType = "frost"; s8.baseDamage = 60; s8.cooldownMs = 6000; s8.manaCost = 15;
    SpiritSkill s9; s9.skillId = 9; s9.classId = 3; s9.nameRu = "Цепная молния"; s9.nameEn = "Chain Lightning"; s9.damageType = "nature"; s9.baseDamage = 40; s9.cooldownMs = 8000; s9.manaCost = 30;
    s_skillsByClass[3] = {s7, s8, s9};

    SpiritSkill s10; s10.skillId = 10; s10.classId = 4; s10.nameRu = "Боевой клич"; s10.nameEn = "Battle Cry"; s10.damageType = "physical"; s10.baseDamage = 0; s10.cooldownMs = 15000; s10.manaCost = 25;
    SpiritSkill s11; s11.skillId = 11; s11.classId = 4; s11.nameRu = "Замедление"; s11.nameEn = "Slow"; s11.damageType = "shadow"; s11.baseDamage = 0; s11.cooldownMs = 8000; s11.manaCost = 20;
    SpiritSkill s12; s12.skillId = 12; s12.classId = 4; s12.nameRu = "Щит группы"; s12.nameEn = "Party Shield"; s12.damageType = "holy"; s12.baseDamage = 0; s12.cooldownMs = 20000; s12.manaCost = 40;
    s_skillsByClass[4] = {s10, s11, s12};
}

} // namespace dm