// SeasonalEvents.h — Сезонные эвенты по временам года
#pragma once
#include <cstdint>
#include <string>

namespace DM {
namespace Events {

// Сезоны
enum Season : uint8_t {
    SEASON_SPRING = 1,
    SEASON_SUMMER = 2,
    SEASON_AUTUMN = 3,
    SEASON_WINTER = 4,
};

// Информация о сезонном эвенте
struct SeasonalEventInfo {
    uint8_t id;
    const char* nameRu;
    const char* nameEn;
    Season season;
    uint8_t startMonth;
    uint8_t startDay;
    uint8_t endMonth;
    uint8_t endDay;
    uint8_t currencyType; // 1=Lumen, 2=Umbra, 3=Splinters, 4=Seasonal
};

inline SeasonalEventInfo GetSeasonalEventInfo(uint8_t eventId) {
    switch (eventId) {
        case 1: return {1, "Праздник Рассвета", "Dawn Festival", SEASON_SPRING, 3, 20, 4, 20, 1};
        case 2: return {2, "Праздник Поглощения", "Hollow Festival", SEASON_SUMMER, 6, 21, 7, 21, 2};
        case 3: return {3, "Праздник Порога", "Threshold Festival", SEASON_AUTUMN, 9, 23, 10, 23, 3};
        case 4: return {4, "Праздник Тьмы", "Darkness Festival", SEASON_WINTER, 12, 21, 1, 21, 4};
        default: return {1, "Праздник Рассвета", "Dawn Festival", SEASON_SPRING, 3, 20, 4, 20, 1};
    }
}

// NPC сезонного эвента
struct SeasonalNpcInfo {
    uint32_t entry;
    const char* nameRu;
    const char* nameEn;
    uint8_t seasonId;
    const char* role; // 'event_host', 'vendor', 'quest_giver'
    float posX, posY, posZ;
    uint32_t mapId;
};

inline SeasonalNpcInfo GetSeasonalNpcInfo(uint32_t npcEntry) {
    switch (npcEntry) {
        case 90020: return {90020, "Весенний вестник", "Spring Herald", 1, "event_host", 200.0f, 100.0f, 10.0f, 1};
        case 90021: return {90021, "Торговец Рассвета", "Dawn Vendor", 1, "vendor", 210.0f, 105.0f, 10.0f, 1};
        case 90022: return {90022, "Садовник Рассвета", "Dawn Gardener", 1, "quest_giver", 195.0f, 95.0f, 10.0f, 1};
        case 90030: return {90030, "Летний вестник", "Summer Herald", 2, "event_host", 300.0f, 150.0f, 15.0f, 2};
        case 90031: return {90031, "Торговец Круга", "Hollow Vendor", 2, "vendor", 310.0f, 155.0f, 15.0f, 2};
        case 90032: return {90032, "Жрец Поглощения", "Hollow Priest", 2, "quest_giver", 295.0f, 145.0f, 15.0f, 2};
        case 90040: return {90040, "Осенний вестник", "Autumn Herald", 3, "event_host", 400.0f, 200.0f, 20.0f, 3};
        case 90041: return {90041, "Торговец Порога", "Threshold Vendor", 3, "vendor", 410.0f, 205.0f, 20.0f, 3};
        case 90042: return {90042, "Торговец масками", "Mask Merchant", 3, "quest_giver", 395.0f, 195.0f, 20.0f, 3};
        case 90050: return {90050, "Зимний вестник", "Winter Herald", 4, "event_host", 500.0f, 250.0f, 25.0f, 4};
        case 90051: return {90051, "Торговец зимы", "Winter Vendor", 4, "vendor", 510.0f, 255.0f, 25.0f, 4};
        case 90052: return {90052, "Хранитель огня", "Fire Keeper", 4, "quest_giver", 495.0f, 245.0f, 25.0f, 4};
        default: return {90020, "Весенний вестник", "Spring Herald", 1, "event_host", 200.0f, 100.0f, 10.0f, 1};
    }
}

// Конфигурация сезонного эвента
struct SeasonalEventConfig {
    bool enabled = true;
    bool autoStart = true;
    uint32_t notificationHoursBefore = 24;
    uint32_t maxParticipants = 1000;
    float rewardMultiplier = 1.0f;
    uint32_t seasonalCurrencyCap = 10000;
    uint32_t eventDurationDays = 30;
};

// Функции
class SeasonalEventMgr {
public:
    static SeasonalEventMgr &Instance();

    // Инициализация: загрузка конфига из БД
    bool LoadConfig();

    // Проверка: идёт ли сезонный эвент
    bool IsActive() const;

    // Получение текущего сезона
    Season GetCurrentSeason() const;

    // Получение активного эвента
    uint8_t GetActiveEventId() const;

    // Регистрация игрока в эвенте
    bool RegisterPlayer(uint64_t playerGuid);

    // Выдача награды за квест
    void GrantQuestReward(uint64_t playerGuid, uint32_t questId);

    // Покупка предмета в магазине
    bool PurchaseShopItem(uint64_t playerGuid, uint32_t itemId);

    // Обновление состояния
    void Update(uint32_t deltaMs);

private:
    SeasonalEventMgr() = default;
    SeasonalEventConfig m_config;
    uint8_t m_activeEventId = 0;
    bool m_isActive = false;
};

} // namespace Events
} // namespace DM