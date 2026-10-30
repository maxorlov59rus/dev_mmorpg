// PvPEvents.h — 10 PvP режимов: еженедельные события
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace DM {
namespace PvP {

// Типы PvP режимов
enum PvPModeType : uint8_t {
    PVP_MODE_SOLO = 1,
    PVP_MODE_GROUP = 2,
    PVP_MODE_GUILD = 3,
    PVP_MODE_FACTION = 4,
    PVP_MODE_MIXED = 5,
};

// ID PvP событий
enum PvPEventId : uint8_t {
    PVP_ARENA_1V1 = 1,
    PVP_ARENA_3V3 = 2,
    PVP_BG_10V10 = 3,
    PVP_CITADEL_SIEGE = 4,
    PVP_FRONTLINE = 5,
    PVP_THRESHOLD_CARAVAN = 6,
    PVP_DUEL_TOURNAMENT = 7,
    PVP_CAPTURE_FLAG = 8,
    PVP_BATTLE_ROYALE = 9,
    PVP_ARENA_2V2 = 10,
};

// Дни недели
enum DayOfWeek : uint8_t {
    SUNDAY = 0,
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
};

// Информация о PvP событии
struct PvPEventInfo {
    uint8_t id;
    const char* nameRu;
    const char* nameEn;
    PvPModeType modeType;
    uint8_t minPlayers;
    uint8_t maxPlayers;
    uint8_t teamSize;
    uint32_t durationMin;
    bool isWeekly;
    DayOfWeek dayOfWeek;
    uint8_t startHour;
    uint8_t startMinute;
    uint32_t mapId;
    uint32_t zoneId;
    uint32_t entryFeeGold;
    uint8_t rewardCurrencyType; // 1=gold, 2=lumen, 3=umbra, 4=splinters, 5=insight
    uint32_t rewardAmount;
    uint32_t bonusRewardAmount;
    uint8_t minLevel;
    uint8_t maxLevel;
    bool isActive;
};

// Конфигурация PvP события
struct PvPEventConfig {
    uint8_t eventId;
    const char* key;
    const char* value;
};

// Функция получения информации о событии
inline PvPEventInfo GetPvPEventInfo(uint8_t eventId) {
    switch (eventId) {
        case PVP_ARENA_1V1:
            return {1, "Арена 1v1", "Arena 1v1", PVP_MODE_SOLO, 2, 2, 1, 10, true, MONDAY, 18, 0, 100, 20, 0, 1, 50, 200, 10, 60, true};
        case PVP_ARENA_3V3:
            return {2, "Арена 3v3", "Arena 3v3", PVP_MODE_GROUP, 6, 6, 3, 15, true, TUESDAY, 18, 0, 101, 21, 0, 1, 100, 500, 10, 60, true};
        case PVP_BG_10V10:
            return {3, "Поле боя 10v10", "Battleground 10v10", PVP_MODE_GROUP, 20, 20, 10, 20, true, WEDNESDAY, 19, 0, 102, 22, 0, 1, 75, 300, 10, 60, true};
        case PVP_CITADEL_SIEGE:
            return {4, "Осада цитадели", "Citadel Siege", PVP_MODE_GUILD, 40, 40, 20, 30, true, THURSDAY, 20, 0, 103, 23, 50, 2, 200, 1000, 20, 60, true};
        case PVP_FRONTLINE:
            return {5, "Фронтальная линия", "Frontline", PVP_MODE_FACTION, 60, 60, 1, 45, true, FRIDAY, 18, 0, 104, 24, 0, 3, 150, 750, 10, 60, true};
        case PVP_THRESHOLD_CARAVAN:
            return {6, "Караван Порога", "Threshold Caravan", PVP_MODE_MIXED, 30, 30, 1, 25, true, SATURDAY, 17, 0, 105, 25, 0, 4, 100, 500, 10, 60, true};
        case PVP_DUEL_TOURNAMENT:
            return {7, "Дуэльный турнир", "Duel Tournament", PVP_MODE_SOLO, 16, 16, 1, 30, true, SUNDAY, 16, 0, 106, 26, 10, 1, 300, 1500, 10, 60, true};
        case PVP_CAPTURE_FLAG:
            return {8, "Захват флага", "Capture the Flag", PVP_MODE_GROUP, 10, 10, 5, 15, true, WEDNESDAY, 20, 0, 107, 27, 0, 1, 80, 400, 10, 60, true};
        case PVP_BATTLE_ROYALE:
            return {9, "Королевская битва", "Battle Royale", PVP_MODE_SOLO, 100, 100, 1, 20, true, SATURDAY, 21, 0, 108, 28, 0, 5, 500, 2500, 10, 60, true};
        case PVP_ARENA_2V2:
            return {10, "Арена 2v2", "Arena 2v2", PVP_MODE_GROUP, 4, 4, 2, 10, true, THURSDAY, 18, 0, 109, 29, 0, 1, 60, 300, 10, 60, true};
        default:
            return {0, "Unknown", "Unknown", PVP_MODE_SOLO, 0, 0, 0, 0, false, SUNDAY, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, false};
    }
}

// Функция получения названия дня недели
inline const char* GetDayName(DayOfWeek day) {
    switch (day) {
        case SUNDAY: return "Воскресенье";
        case MONDAY: return "Понедельник";
        case TUESDAY: return "Вторник";
        case WEDNESDAY: return "Среда";
        case THURSDAY: return "Четверг";
        case FRIDAY: return "Пятница";
        case SATURDAY: return "Суббота";
        default: return "Unknown";
    }
}

// Функция получения названия типа режима
inline const char* GetModeTypeName(PvPModeType type) {
    switch (type) {
        case PVP_MODE_SOLO: return "Соло";
        case PVP_MODE_GROUP: return "Группа";
        case PVP_MODE_GUILD: return "Гильдия";
        case PVP_MODE_FACTION: return "Фракция";
        case PVP_MODE_MIXED: return "Смешанный";
        default: return "Unknown";
    }
}

} // namespace PvP
} // namespace DM