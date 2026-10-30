// QuestSystem_27_50.h — Квестовая система 27-50 для всех фракций и классов
// 3 фракции x 6 классов x 24 квеста = 432 квеста
#pragma once
#include <cstdint>

namespace DM {
namespace Game {

// Зоны 27-50
enum ZoneId27_50 : uint8_t {
    ZONE_RIFT = 11,           // Разлом Шип (27-31)
    ZONE_RIFT_CITADEL = 12,   // Цитадель Искажённых (32-36)
    ZONE_DEAD_LIMITS = 13,    // Мёртвые Пределы (37-41)
    ZONE_ANCIENT_RUINS = 14,  // Руины Древних (42-46)
    ZONE_THE_MAW = 15,        // Глотка (47-50)
};

// Базовый ID квеста 27-50: 20000 + (faction-1)*10000 + class*100 + quest_number
// Пример: Фракция 1, Воин, Квест 27 = 20000 + 0 + 100 + 27 = 20127

// Базовые ID мобов 27-50: 60000 + (faction-1)*1000 + class*100 + (quest-26)
// Базовые ID NPC 27-50: 80000 + (faction-1)*1000 + class*100 + 27
// Базовые ID предметов: 12000 + (faction-1)*100 + class*10 + (quest-26)

// Характеристики мобов 27-50
struct MobStats27_50 {
    uint8_t level;
    float dmg_modifier;
    float health_modifier;
    uint32_t mingold;
    uint32_t maxgold;
    uint8_t unit_flags;
    bool isElite;
    bool isBoss;
};

inline MobStats27_50 GetMobStats27_50(uint8_t level) {
    MobStats27_50 stats;
    stats.level = level;
    stats.dmg_modifier = 1.5f + (level - 27) * 0.12f;
    stats.health_modifier = 2.0f + (level - 27) * 0.2f;
    stats.mingold = 50 + (level - 27) * 10;
    stats.maxgold = 100 + (level - 27) * 20;
    stats.unit_flags = 1; // Aggressive
    stats.isElite = (level >= 30);
    stats.isBoss = (level >= 40);
    if (stats.isElite) stats.unit_flags |= 2; // Elite flag
    if (stats.isBoss) stats.unit_flags |= 8;  // Boss flag
    return stats;
}

// Награды за квесты 27-50
struct QuestReward27_50 {
    uint32_t money;
    uint32_t reputation;
    uint8_t difficulty; // 0=Normal, 1=Heroic, 2=Epic, 3=Legendary
};

inline QuestReward27_50 GetQuestReward27_50(uint8_t questNum) {
    QuestReward27_50 reward;
    reward.money = 2000 + (questNum - 27) * 300;
    reward.reputation = 500 + (questNum - 27) * 50;
    if (questNum >= 47) reward.difficulty = 3;      // Legendary
    else if (questNum >= 44) reward.difficulty = 2; // Epic
    else if (questNum >= 30) reward.difficulty = 1; // Heroic
    else reward.difficulty = 0;                      // Normal
    return reward;
}

// Функции для получения ID
inline uint32_t GetQuestId27_50(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 20000 + (faction - 1) * 10000 + class_id * 100 + quest_num;
}

inline uint32_t GetMobId27_50(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 60000 + (faction - 1) * 1000 + class_id * 100 + (quest_num - 26);
}

inline uint32_t GetNpcId27_50(uint8_t faction, uint8_t class_id) {
    return 80000 + (faction - 1) * 1000 + class_id * 100 + 27;
}

inline uint32_t GetItemId27_50(uint8_t faction, uint8_t class_id, uint8_t quest_num) {
    return 12000 + (faction - 1) * 100 + class_id * 10 + (quest_num - 26);
}

// Типы квестов 27-50
enum QuestType27_50 : uint8_t {
    QUEST_TYPE_KILL = 1,
    QUEST_TYPE_GATHER = 2,
    QUEST_TYPE_EXPLORE = 3,
    QUEST_TYPE_TALK = 4,
    QUEST_TYPE_INSTANCE = 5,
    QUEST_TYPE_RAID = 6,
};

// Боссы 27-50
struct BossInfo {
    uint32_t entry;
    const char* name;
    uint8_t level;
    uint32_t healthMod;
    uint32_t dmgMod;
    const char* ability;
};

inline BossInfo GetBossInfo(uint8_t zone) {
    switch (zone) {
        case ZONE_RIFT:           return {60001, "Страж Разлома", 30, 5, 2, "Теневой удар"};
        case ZONE_RIFT_CITADEL:   return {60002, "Повелитель Искажённых", 36, 8, 3, "Искажение реальности"};
        case ZONE_DEAD_LIMITS:    return {60003, "Древний Лич", 42, 10, 4, "Некромантия"};
        case ZONE_ANCIENT_RUINS:  return {60004, "Повелитель Древних", 46, 12, 5, "Древняя магия"};
        case ZONE_THE_MAW:        return {60005, "Фрагмент Чёрной Материи", 50, 20, 8, "Поглощение"};
        default:                  return {0, "Unknown", 0, 1, 1, "None"};
    }
}

} // namespace Game
} // namespace DM