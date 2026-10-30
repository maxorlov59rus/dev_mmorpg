// SpiritMgr.h — Система духов-питомцев
#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <random>

namespace dm {

enum class SpiritQuality : uint8_t {
    Common = 1,
    Rare = 2,
    Epic = 3,
    Legendary = 4
};

enum class SpiritClass : uint8_t {
    Guardian = 1,
    Healer = 2,
    Destroyer = 3,
    Support = 4
};

struct SpiritSkill {
    uint32_t skillId;
    uint8_t classId;
    std::string nameRu;
    std::string nameEn;
    std::string damageType;
    uint32_t baseDamage;
    uint32_t cooldownMs;
    uint32_t manaCost;
};

struct Spirit {
    uint64_t spiritId;
    uint32_t playerGuid;
    SpiritQuality quality;
    SpiritClass spiritClass;
    uint32_t level;
    uint64_t experience;
    std::vector<uint32_t> skills;
    std::unordered_map<uint32_t, uint32_t> skillLevels; // skillId -> level
    bool isActive;
    uint32_t rollsUsedToday;
    uint32_t lastRollDate;
    uint32_t createdAt;
};

class SpiritMgr {
public:
    static SpiritMgr &Instance();

    // Крутки духов
    Spirit RollSpirit(uint32_t playerGuid);
    bool CanRoll(uint32_t playerGuid) const;
    uint32_t GetRollsRemaining(uint32_t playerGuid) const;
    void ResetDailyRolls(uint32_t playerGuid);

    // Управление духами
    bool AddSpirit(uint32_t playerGuid, const Spirit &spirit);
    bool RemoveSpirit(uint32_t playerGuid, uint64_t spiritId);
    bool SetActiveSpirit(uint32_t playerGuid, uint64_t spiritId);
    Spirit *GetActiveSpirit(uint32_t playerGuid);
    std::vector<Spirit> GetSpirits(uint32_t playerGuid) const;
    uint32_t GetSpiritCount(uint32_t playerGuid) const;

    // Опыт и уровень
    bool AddExperience(uint32_t playerGuid, uint64_t spiritId, uint64_t exp);
    bool LevelUp(uint32_t playerGuid, uint64_t spiritId);
    void SyncLevelWithPlayer(uint32_t playerGuid, uint8_t playerLevel);
    bool IsSpiritLevelSynced(uint32_t playerGuid, uint64_t spiritId, uint8_t playerLevel) const;

    // Навыки
    std::vector<SpiritSkill> GetSkillsForClass(SpiritClass cls) const;
    uint32_t GetSkillSlotCount(SpiritQuality quality) const;

    // Продажа (запрещена)
    bool CanSellSpirit() const { return false; }

    // Конфигурация
    uint32_t GetDailyRolls() const { return 10; }
    float GetHigherQualityChance() const { return 0.10f; }
    uint32_t GetMaxSpirits() const { return 5; }

    // Загрузка данных
    void LoadSpiritSkills();
};

} // namespace dm