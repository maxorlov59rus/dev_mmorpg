// QuestSystem_16_26.h — Квестовая система 16-26 для всех фракций и классов
// 3 фракции x 6 классов x 11 квестов = 198 квестов
#pragma once
#include <cstdint>

namespace DM {
namespace Game {

// Фракции
enum FactionId : uint8_t {
    FACTION_COVENANT_DAWN = 1,      // Орден Рассвета
    FACTION_CIRCLE_HOLLOW = 2,      // Круг Поглощения
    FACTION_THRESHOLD_GUILDS = 3,   // Гильдия Порога
};

// Классы
enum ClassId : uint8_t {
    CLASS_WARRIOR = 1,
    CLASS_PRIEST = 2,
    CLASS_MAGE = 3,
    CLASS_ROGUE = 4,
    CLASS_HUNTER = 5,
    CLASS_WARLOCK = 6,
};

// Базовый ID квеста: 10000 + (faction-1)*10000 + class*100 + quest_number
// Пример: Фракция 1, Воин, Квест 16 = 10000 + 0 + 100 + 16 = 10116

// Уровни сложности квестов
enum QuestDifficulty : uint8_t {
    DIFFICULTY_NORMAL = 0,
    DIFFICULTY_HEROIC = 1,
    DIFFICULTY_EPIC = 2,
    DIFFICULTY_LEGENDARY = 3,
};

// Базовые ID мобов: 50000 + (faction-1)*1000 + class*100 + (quest-15)
// Базовые ID NPC: 70000 + (faction-1)*1000 + class*100 + 16
// Базовые ID предметов: 11000 + (faction-1)*100 + class*10 + (quest-15)

// Типы квестов
enum QuestType : uint8_t {
    QUEST_TYPE_KILL = 1,
    QUEST_TYPE_GATHER = 2,
    QUEST_TYPE_EXPLORE = 3,
    QUEST_TYPE_TALK = 4,
    QUEST_TYPE_INSTANCE = 5,
};

// Типы мобов
enum MobType : uint8_t {
    MOB_TYPE_NORMAL = 4,
    MOB_TYPE_ELITE = 6,
    MOB_TYPE_BOSS = 8,
    MOB_TYPE_RARE = 4 | 4, // Rare flag
};

// Характеристики мобов по уровню
struct MobStats {
    uint8_t level;
    float dmg_modifier;
    float health_modifier;
    uint32_t mingold;
    uint32_t maxgold;
    uint8_t unit_flags;
};

// Базовые характеристики для мобов 16-26
inline MobStats GetMobStats(uint8_t level) {
    MobStats stats;
    stats.level = level;
    stats.dmg_modifier = 1.0f + (level - 16) * 0.1f;
    stats.health_modifier = 1.0f + (level - 16) * 0.15f;
    stats.mingold = 10 + (level - 16) * 5;
    stats.maxgold = 25 + (level - 16) * 10;
    stats.unit_flags = 1; // Aggressive by default
    if (level >= 20) stats.unit_flags |= 8; // Elite for 20+
    return stats;
}

// Механики боя
namespace Combat {
    // Типы урона
    enum DamageType : uint8_t {
        DAMAGE_PHYSICAL = 0,
        DAMAGE_MAGICAL = 1,
        DAMAGE_HOLY = 2,
        DAMAGE_SHADOW = 3,
        DAMAGE_FIRE = 4,
        DAMAGE_FROST = 5,
        DAMAGE_NATURE = 6,
    };

    // Способности мобов
    struct MobAbility {
        uint32_t spell_id;
        uint8_t cooldown; // seconds
        uint8_t chance;   // % chance to use
        DamageType damage_type;
    };

    // Базовые способности для мобов 16-26
    inline MobAbility GetMobAbility(uint8_t mob_level) {
        MobAbility ability;
        if (mob_level < 20) {
            ability.spell_id = 1000 + mob_level;
            ability.cooldown = 5;
            ability.chance = 30;
            ability.damage_type = DAMAGE_PHYSICAL;
        } else if (mob_level < 24) {
            ability.spell_id = 2000 + mob_level;
            ability.cooldown = 8;
            ability.chance = 40;
            ability.damage_type = DAMAGE_MAGICAL;
        } else {
            ability.spell_id = 3000 + mob_level;
            ability.cooldown = 10;
            ability.chance = 50;
            ability.damage_type = DAMAGE_SHADOW;
        }
        return ability;
    }
}

// Функции для получения ID
inline uint32_t GetQuestId(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 10000 + (faction - 1) * 10000 + class_id * 100 + quest_num;
}

inline uint32_t GetMobId(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 50000 + (faction - 1) * 1000 + class_id * 100 + (quest_num - 15);
}

inline uint32_t GetNpcId(uint8_t faction, uint8_t class_id) {
    return 70000 + (faction - 1) * 1000 + class_id * 100 + 16;
}

inline uint32_t GetItemId(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 11000 + (faction - 1) * 100 + class_id * 10 + (quest_num - 15);
}

} // namespace Game
} // namespace DM