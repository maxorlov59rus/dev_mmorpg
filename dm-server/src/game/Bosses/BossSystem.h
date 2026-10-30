// BossSystem.h — Система боссов для квестов 27-50
// Боссы: Страж Разлома, Повелитель Искажённых, Древний Лич, Повелитель Древних, Фрагмент Чёрной Материи
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace DM {
namespace Game {

// Типы урона
enum BossDamageType : uint8_t {
    BOSS_DMG_PHYSICAL = 0,
    BOSS_DMG_MAGICAL = 1,
    BOSS_DMG_HOLY = 2,
    BOSS_DMG_SHADOW = 3,
    BOSS_DMG_FIRE = 4,
    BOSS_DMG_FROST = 5,
    BOSS_DMG_NATURE = 6,
};

// Фазы босса
enum BossPhase : uint8_t {
    BOSS_PHASE_1 = 1,
    BOSS_PHASE_2 = 2,
    BOSS_PHASE_ENRAGE = 3,
};

// Способность босса
struct BossAbility {
    uint32_t spellId;
    const char* name;
    BossDamageType damageType;
    uint32_t baseDamage;
    uint32_t cooldownSec;
    uint8_t chancePct;
    BossPhase phase;
    bool isAoe;
    float radius;
};

// Информация о боссе
struct BossInfo {
    uint32_t entry;
    const char* name;
    const char* subname;
    uint8_t level;
    float dmgModifier;
    float healthModifier;
    uint32_t mingold;
    uint32_t maxgold;
    uint32_t mapId;
    uint32_t zoneId;
    float posX, posY, posZ;
    uint32_t respawnTimeSec;
    bool isInstance;
    uint8_t phase2HpPct;
    uint32_t enrageTimerSec;
    std::vector<BossAbility> abilities;
};

// Боссы 27-50
inline BossInfo GetBossInfo(uint32_t bossEntry) {
    switch (bossEntry) {
        case 60001: // Страж Разлома
            return {
                60001, "Страж Разлома", "Хранитель Разлома Шип", 30,
                2.0f, 5.0f, 100, 200, 11, 11, 500.0f, 250.0f, 30.0f, 3600, false, 50, 300,
                {
                    {6000101, "Теневой удар", BOSS_DMG_SHADOW, 150, 5, 40, BOSS_PHASE_1, false, 0},
                    {6000102, "Призыв теневых волков", BOSS_DMG_PHYSICAL, 0, 15, 20, BOSS_PHASE_1, false, 0},
                    {6000103, "Усиленный теневой удар", BOSS_DMG_SHADOW, 300, 8, 100, BOSS_PHASE_2, false, 0},
                }
            };
        case 60002: // Повелитель Искажённых
            return {
                60002, "Повелитель Искажённых", "Владыка Цитадели", 36,
                3.0f, 8.0f, 200, 400, 12, 12, 200.0f, 100.0f, 15.0f, 3600, true, 40, 420,
                {
                    {6000201, "Искажение реальности", BOSS_DMG_MAGICAL, 200, 8, 35, BOSS_PHASE_1, true, 10},
                    {6000202, "Призыв аддов", BOSS_DMG_PHYSICAL, 0, 20, 15, BOSS_PHASE_1, false, 0},
                    {6000203, "Телепортация", BOSS_DMG_PHYSICAL, 0, 25, 10, BOSS_PHASE_1, false, 0},
                    {6000204, "Массовое искажение", BOSS_DMG_MAGICAL, 400, 12, 100, BOSS_PHASE_2, true, 20},
                }
            };
        case 60003: // Древний Лич
            return {
                60003, "Древний Лич", "Повелитель Мёртвых Пределов", 42,
                4.0f, 10.0f, 300, 600, 13, 13, 400.0f, 200.0f, 20.0f, 3600, false, 30, 480,
                {
                    {6000301, "Некромантия", BOSS_DMG_SHADOW, 250, 6, 45, BOSS_PHASE_1, false, 0},
                    {6000302, "Призыв нежити", BOSS_DMG_PHYSICAL, 0, 12, 25, BOSS_PHASE_1, false, 0},
                    {6000303, "Ледяная стрела", BOSS_DMG_FROST, 180, 10, 30, BOSS_PHASE_1, false, 0},
                    {6000304, "Массовое воскрешение нежити", BOSS_DMG_PHYSICAL, 0, 20, 100, BOSS_PHASE_2, true, 30},
                }
            };
        case 60004: // Повелитель Древних
            return {
                60004, "Повелитель Древних", "Владыка Руин", 46,
                5.0f, 12.0f, 400, 800, 14, 14, 600.0f, 300.0f, 25.0f, 3600, false, 25, 540,
                {
                    {6000401, "Древняя магия", BOSS_DMG_MAGICAL, 300, 7, 40, BOSS_PHASE_1, false, 0},
                    {6000402, "Призыв големов", BOSS_DMG_PHYSICAL, 0, 18, 20, BOSS_PHASE_1, false, 0},
                    {6000403, "Каменный дождь", BOSS_DMG_PHYSICAL, 150, 15, 25, BOSS_PHASE_1, true, 15},
                    {6000404, "Апокалипсис", BOSS_DMG_MAGICAL, 600, 20, 100, BOSS_PHASE_2, true, 30},
                }
            };
        case 60005: // Фрагмент Чёрной Материи
            return {
                60005, "Фрагмент Чёрной Материи", "Сознание Пустоты", 50,
                8.0f, 20.0f, 1000, 2000, 15, 15, 800.0f, 400.0f, 35.0f, 86400, true, 20, 600,
                {
                    {6000501, "Поглощение", BOSS_DMG_SHADOW, 400, 5, 50, BOSS_PHASE_1, false, 0},
                    {6000502, "Искажение пространства", BOSS_DMG_MAGICAL, 250, 10, 30, BOSS_PHASE_1, true, 20},
                    {6000503, "Призыв Полых", BOSS_DMG_PHYSICAL, 0, 15, 20, BOSS_PHASE_1, false, 0},
                    {6000504, "Абсолютное поглощение", BOSS_DMG_SHADOW, 800, 15, 100, BOSS_PHASE_2, true, 40},
                }
            };
        default:
            return {0, "Unknown", "Unknown", 0, 1.0f, 1.0f, 0, 0, 0, 0, 0, 0, 0, 0, false, 50, 0, {}};
    }
}

// Функция получения имени босса по entry
inline const char* GetBossName(uint32_t bossEntry) {
    switch (bossEntry) {
        case 60001: return "Страж Разлома";
        case 60002: return "Повелитель Искажённых";
        case 60003: return "Древний Лич";
        case 60004: return "Повелитель Древних";
        case 60005: return "Фрагмент Чёрной Материи";
        default: return "Unknown Boss";
    }
}

// Функция получения фазы босса по HP
inline BossPhase GetBossPhase(uint32_t bossEntry, float hpPercent) {
    BossInfo info = GetBossInfo(bossEntry);
    if (hpPercent <= info.phase2HpPct) {
        return BOSS_PHASE_2;
    }
    return BOSS_PHASE_1;
}

} // namespace Game
} // namespace DM