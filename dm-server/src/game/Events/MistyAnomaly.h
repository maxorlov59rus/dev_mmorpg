// MistyAnomaly.h — Еженедельный PvE/PvP эвент "Туманная Аномалия"
#pragma once
#include <cstdint>
#include <string>

namespace DM {
namespace Events {

// Типы мобов аномалии
enum MistyMobType : uint8_t {
    MISTY_GHOST = 1,
    MISTY_GUARDIAN = 2,
    MISTY_BOSS = 3,
    MISTY_NIGHTMARE = 4,
    MISTY_ELEMENTAL = 5,
};

// Информация о мобе аномалии
struct MistyMobInfo {
    uint32_t entry;
    const char* nameRu;
    const char* nameEn;
    uint8_t level;
    uint8_t type; // 4=normal, 6=elite, 8=boss
    float hpModifier;
    float dmgModifier;
    uint32_t spawnCount;
    uint32_t spawnIntervalSec;
    bool isBoss;
    uint32_t lootTableId;
};

inline MistyMobInfo GetMistyMobInfo(uint8_t mobType) {
    switch (mobType) {
        case MISTY_GHOST:
            return {70001, "Туманный призрак", "Misty Ghost", 15, 4, 1.0f, 1.0f, 10, 30, false, 70001};
        case MISTY_GUARDIAN:
            return {70002, "Туманный страж", "Misty Guardian", 20, 6, 2.0f, 1.5f, 5, 60, false, 70002};
        case MISTY_BOSS:
            return {70003, "Туманный босс", "Misty Boss", 25, 6, 5.0f, 2.0f, 1, 300, true, 70003};
        case MISTY_NIGHTMARE:
            return {70004, "Туманный кошмар", "Misty Nightmare", 30, 8, 8.0f, 3.0f, 1, 0, true, 70004};
        case MISTY_ELEMENTAL:
            return {70005, "Туманный элементаль", "Misty Elemental", 18, 4, 1.2f, 1.1f, 8, 45, false, 70005};
        default:
            return {70001, "Туманный призрак", "Misty Ghost", 15, 4, 1.0f, 1.0f, 10, 30, false, 70001};
    }
}

// Конфигурация эвента
struct MistyAnomalyConfig {
    bool enabled = true;
    uint8_t minPlayerLevel = 10;
    uint8_t maxPlayers = 50;
    uint32_t durationMin = 60;
    float fogDensity = 0.8f;
    float visibilityRange = 30.0f;
    bool pvpEnabled = true;
    uint32_t mobSpawnIntervalSec = 30;
    uint32_t bossSpawnIntervalSec = 300;
    float anomalyCenterX = 500.0f;
    float anomalyCenterY = 250.0f;
    float anomalyCenterZ = 10.0f;
    float anomalyRadius = 200.0f;
    uint8_t rewardCurrencyType = 1; // 1=gold
    uint32_t participationReward = 50;
    uint32_t killScore = 10;
    uint32_t pvpKillScore = 25;
    uint32_t bossKillScore = 100;
};

// Расписание
struct MistyAnomalySchedule {
    uint8_t dayOfWeek = 0; // Sunday
    uint8_t startHour = 20;
    uint8_t startMinute = 0;
    uint32_t durationMin = 60;
    bool isActive = true;
};

// Информация об игроке в эвенте
struct MistyAnomalyPlayer {
    uint64_t playerGuid = 0;
    std::string playerName;
    uint8_t level = 0;
    uint8_t factionId = 0;
    uint32_t score = 0;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    uint32_t mobKills = 0;
    uint32_t bossKills = 0;
    bool isActive = true;
};

// Награды
struct MistyAnomalyReward {
    uint8_t placement; // 0=participation, 1=1st, 2=2nd, 3=3rd
    uint8_t currencyType;
    uint32_t amount;
    uint32_t itemId;
    uint32_t itemCount;
    uint32_t titleId;
};

inline MistyAnomalyReward GetMistyAnomalyReward(uint8_t placement) {
    switch (placement) {
        case 1: return {1, 1, 1000, 15007, 1, 2001};
        case 2: return {2, 1, 500, 15005, 1, 0};
        case 3: return {3, 1, 250, 15005, 1, 0};
        default: return {0, 1, 50, 15001, 3, 0};
    }
}

// Функции
class MistyAnomalyMgr {
public:
    static MistyAnomalyMgr &Instance();

    // Инициализация: загрузка конфига из БД
    bool LoadConfig();

    // Запуск эвента
    bool StartEvent();

    // Завершение эвента
    void EndEvent();

    // Регистрация игрока в эвенте
    bool RegisterPlayer(uint64_t playerGuid);

    // Выход игрока из эвента
    void UnregisterPlayer(uint64_t playerGuid);

    // Обновление состояния эвента
    void Update(uint32_t deltaMs);

    // Начисление очков за убийство моба
    void OnMobKill(uint64_t playerGuid, uint32_t mobEntry);

    // Начисление очков за убийство игрока
    void OnPlayerKill(uint64_t killerGuid, uint64_t victimGuid);

    // Начисление очков за убийство босса
    void OnBossKill(uint64_t playerGuid, uint32_t bossEntry);

    // Выдача наград
    void DistributeRewards();

    // Проверка: идёт ли эвент
    bool IsActive() const;

    // Получение списка игроков
    std::vector<MistyAnomalyPlayer> GetPlayers() const;

    // Получение конфига
    const MistyAnomalyConfig& GetConfig() const;

private:
    MistyAnomalyMgr() = default;
    MistyAnomalyConfig m_config;
    MistyAnomalySchedule m_schedule;
    std::vector<MistyAnomalyPlayer> m_players;
    bool m_isActive = false;
    uint64_t m_startTime = 0;
    uint64_t m_endTime = 0;
    uint32_t m_anomalyCenterX = 500;
    uint32_t m_anomalyCenterY = 250;
    uint32_t m_anomalyCenterZ = 10;
    float m_anomalyRadius = 200.0f;
};

} // namespace Events
} // namespace DM