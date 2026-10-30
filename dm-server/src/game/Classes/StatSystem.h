// StatSystem.h — Система атрибутов и свободных очков
// GDD §2: STR/AGI/INT/VIT/SPI + 5 свободных очков/level
#pragma once
#include <cstdint>

namespace DM {

// Атрибуты персонажа
enum StatType : uint8_t {
    STAT_STRENGTH = 0,    // Сила (STR) — физический урон, парирование
    STAT_AGILITY = 1,     // Ловкость (AGI) — крит, уклонение, скорость атаки
    STAT_INTELLECT = 2,   // Интеллект (INT) — магический урон, мана
    STAT_VITALITY = 3,    // Выносливость (VIT) — HP, броня
    STAT_SPIRIT = 4,      // Дух (SPI) — регенерация маны/энергии, сопротивление
    STAT_COUNT = 5,
};

// Базовые статы по классу (авто-распределение)
struct BaseStats {
    uint8_t strength = 10;
    uint8_t agility = 10;
    uint8_t intellect = 10;
    uint8_t vitality = 10;
    uint8_t spirit = 10;
};

// Получение базовых статов по классу
inline BaseStats GetBaseStatsForClass(uint8_t classId) {
    BaseStats stats;
    switch (classId) {
        case 1: // Warrior
            stats.strength = 15; stats.agility = 10; stats.intellect = 5;
            stats.vitality = 15; stats.spirit = 5;
            break;
        case 2: // Priest
            stats.strength = 5; stats.agility = 8; stats.intellect = 15;
            stats.vitality = 10; stats.spirit = 12;
            break;
        case 3: // Mage
            stats.strength = 5; stats.agility = 8; stats.intellect = 18;
            stats.vitality = 8; stats.spirit = 9;
            break;
        case 4: // Rogue
            stats.strength = 8; stats.agility = 18; stats.intellect = 5;
            stats.vitality = 10; stats.spirit = 9;
            break;
        case 5: // Hunter
            stats.strength = 8; stats.agility = 16; stats.intellect = 6;
            stats.vitality = 10; stats.spirit = 10;
            break;
        case 6: // Warlock
            stats.strength = 5; stats.agility = 8; stats.intellect = 16;
            stats.vitality = 10; stats.spirit = 11;
            break;
    }
    return stats;
}

// Свободные очки атрибутов
struct FreeStatPoints {
    uint8_t available = 0;  // доступные очки
    uint8_t spent[STAT_COUNT] = {0, 0, 0, 0, 0}; // потраченные очки по атрибутам

    // Добавить очки при level-up (5 за уровень)
    void OnLevelUp() {
        available += 5;
    }

    // Потратить очко на атрибут
    bool SpendPoint(StatType stat) {
        if (available == 0) return false;
        if (stat >= STAT_COUNT) return false;
        available--;
        spent[stat]++;
        return true;
    }

    // Сбросить все очки (за плату)
    void Reset() {
        uint8_t total = available;
        for (int i = 0; i < STAT_COUNT; i++) {
            total += spent[i];
            spent[i] = 0;
        }
        available = total;
    }

    // Получить итоговый атрибут
    uint8_t GetFinalStat(StatType stat, const BaseStats &base) const {
        switch (stat) {
            case STAT_STRENGTH: return base.strength + spent[stat];
            case STAT_AGILITY: return base.agility + spent[stat];
            case STAT_INTELLECT: return base.intellect + spent[stat];
            case STAT_VITALITY: return base.vitality + spent[stat];
            case STAT_SPIRIT: return base.spirit + spent[stat];
            default: return 0;
        }
    }
};

// Конвертация атрибутов в боевые статы
struct CombatStats {
    uint32_t attackPower = 0;
    uint32_t spellPower = 0;
    uint32_t critChance = 0;      // в тысячных (1000 = 10%)
    uint32_t dodgeChance = 0;      // в тысячных
    uint32_t hpMax = 0;
    uint32_t manaMax = 0;
    uint32_t armor = 0;
    uint32_t manaRegen = 0;

    static CombatStats FromAttributes(const BaseStats &base, const FreeStatPoints &points) {
        CombatStats cs;
        uint8_t str = base.strength + points.spent[STAT_STRENGTH];
        uint8_t agi = base.agility + points.spent[STAT_AGILITY];
        uint8_t intel = base.intellect + points.spent[STAT_INTELLECT];
        uint8_t vit = base.vitality + points.spent[STAT_VITALITY];
        uint8_t spi = base.spirit + points.spent[STAT_SPIRIT];

        cs.attackPower = str * 2;
        cs.spellPower = intel * 2;
        cs.critChance = agi * 5;      // 0.5% per AGI
        cs.dodgeChance = agi * 3;     // 0.3% per AGI
        cs.hpMax = vit * 10;
        cs.manaMax = intel * 15;
        cs.armor = vit * 5;
        cs.manaRegen = spi * 2;
        return cs;
    }
};

} // namespace DM