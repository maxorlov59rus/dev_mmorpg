// AutoPvP.h — Система авто-PvP с ботами-клонами оффлайн игроков
// Арена Призраков: автоматические бои с клонами, очки веры, обменник оружия
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace DM {
namespace PvP {

// Уровни сложности авто-PvP
enum AutoPvPDifficulty : uint8_t {
    AUTO_PVP_EASY = 1,
    AUTO_PVP_MEDIUM = 2,
    AUTO_PVP_HARD = 3,
};

// Информация о сложности
struct AutoPvPDifficultyInfo {
    uint8_t id;
    const char* nameRu;
    const char* nameEn;
    float cloneStatMultiplier;
    uint8_t cloneAiLevel; // 1=easy, 2=medium, 3=hard
    uint32_t faithPointsReward;
    uint32_t bonusFaithPoints;
    uint8_t minPlayerLevel;
    uint32_t entryFeeGold;
    bool isActive;
};

inline AutoPvPDifficultyInfo GetAutoPvPDifficulty(uint8_t diffId) {
    switch (diffId) {
        case AUTO_PVP_EASY:
            return {1, "Лёгкий", "Easy", 0.8f, 1, 10, 5, 10, 0, true};
        case AUTO_PVP_MEDIUM:
            return {2, "Средний", "Medium", 1.0f, 2, 25, 10, 20, 50, true};
        case AUTO_PVP_HARD:
            return {3, "Сложный", "Hard", 1.2f, 3, 50, 25, 30, 100, true};
        default:
            return {1, "Лёгкий", "Easy", 0.8f, 1, 10, 5, 10, 0, true};
    }
}

// Профиль ИИ клона
enum CloneAIProfile : uint8_t {
    AI_PASSIVE = 1,    // Пассивный (только атакует)
    AI_AGGRESSIVE = 2, // Агрессивный (атакует + использует способности)
    AI_TACTICAL = 3,   // Тактический (уклоняется, использует кулдауны)
};

// Данные клона игрока
struct PlayerCloneData {
    uint64_t cloneId = 0;
    uint64_t playerGuid = 0;
    std::string playerName;
    uint8_t classId = 0;
    uint8_t factionId = 0;
    uint8_t level = 0;
    std::string equipmentJson; // JSON экипировка
    std::string statsJson;     // JSON статы
    CloneAIProfile aiProfile = AI_PASSIVE;
    bool isAvailable = true;
    uint32_t timesUsed = 0;
};

// Результат матча
enum AutoPvPResult : uint8_t {
    AUTO_PVP_LOSS = 0,
    AUTO_PVP_WIN = 1,
    AUTO_PVP_DRAW = 2,
};

// Запись матча
struct AutoPvPMatch {
    uint64_t matchId = 0;
    uint64_t playerGuid = 0;
    uint64_t cloneGuid = 0;
    std::string cloneName;
    uint8_t difficulty = 1;
    AutoPvPResult result = AUTO_PVP_LOSS;
    uint32_t faithPointsEarned = 0;
    uint32_t matchDurationSec = 0;
    uint64_t timestamp = 0;
};

// Очки веры игрока
struct FaithPoints {
    uint64_t playerGuid = 0;
    uint32_t amount = 0;
    uint32_t totalEarned = 0;
    uint32_t totalSpent = 0;
};

// Предмет обменника
struct FaithShopItem {
    uint32_t id = 0;
    uint32_t itemEntry = 0;
    const char* nameRu;
    const char* nameEn;
    uint32_t faithCost = 0;
    uint8_t requiredLevel = 10;
    uint8_t requiredDifficulty = 1;
    uint32_t itemLevel = 0;
    const char* uniqueStatsJson;
    bool isAvailable = true;
    int32_t stock = -1; // -1 = unlimited
};

// Функции получения данных
inline FaithShopItem GetFaithShopItem(uint32_t itemId) {
    switch (itemId) {
        case 1: return {1, 14001, "Клинок Призрака", "Ghost Blade", 100, 10, 1, 15, "{\"damage_vs_clones\":0.10}", true, -1};
        case 2: return {2, 14002, "Посох Призрака", "Ghost Staff", 150, 15, 1, 20, "{\"spell_power_bonus\":0.15}", true, -1};
        case 3: return {3, 14003, "Лук Призрака", "Ghost Bow", 120, 12, 1, 18, "{\"attack_speed_bonus\":0.12}", true, -1};
        case 4: return {4, 14004, "Кинжал Призрака", "Ghost Dagger", 200, 20, 2, 25, "{\"crit_damage_bonus\":0.20}", true, -1};
        case 5: return {5, 14005, "Молот Призрака", "Ghost Hammer", 300, 25, 2, 30, "{\"damage_vs_elite\":0.25}", true, -1};
        case 6: return {6, 14006, "Скипетр Призрака", "Ghost Scepter", 250, 20, 2, 28, "{\"healing_bonus\":0.30}", true, -1};
        case 7: return {7, 14007, "Коса Призрака", "Ghost Scythe", 400, 30, 3, 35, "{\"damage_bonus\":0.15,\"lifesteal\":0.10}", true, -1};
        case 8: return {8, 14008, "Клинок Пустоты", "Void Blade", 1000, 40, 3, 45, "{\"damage_bonus\":0.50,\"crit_chance\":0.20}", true, 10};
        case 9: return {9, 14009, "Посох Пустоты", "Void Staff", 1000, 40, 3, 45, "{\"spell_power_bonus\":0.60}", true, 10};
        case 10: return {10, 14010, "Лук Пустоты", "Void Bow", 1000, 40, 3, 45, "{\"attack_speed_bonus\":0.40,\"crit_chance\":0.25}", true, 10};
        default: return {0, 0, "Unknown", "Unknown", 0, 0, 0, 0, "{}", false, 0};
    }
}

// Константы
constexpr uint32_t AUTO_PVP_MATCH_DURATION_SEC = 120;
constexpr uint32_t AUTO_PVP_RESPAWN_DELAY_SEC = 5;
constexpr uint32_t AUTO_PVP_DAILY_CAP_FAITH = 500;
constexpr uint32_t AUTO_PVP_MAX_MATCHES_PER_DAY = 10;
constexpr float AUTO_PVP_CLONE_STAT_VARIANCE = 0.1f;
constexpr uint32_t AUTO_PVP_AI_THINK_INTERVAL_MS = 500;

// Функции
class AutoPvPMgr {
public:
    static AutoPvPMgr &Instance();

    // Инициализация: загрузка клонов из БД
    bool LoadClones();

    // Выбор клона для игрока (по уровню/фракции/случайно)
    PlayerCloneData SelectClone(uint64_t playerGuid, uint8_t difficulty, const char* mode);

    // Запуск матча
    bool StartMatch(uint64_t playerGuid, uint8_t difficulty);

    // Обработка результата матча
    void OnMatchComplete(uint64_t matchId, AutoPvPResult result);

    // Начисление очков веры
    void GrantFaithPoints(uint64_t playerGuid, uint32_t amount);

    // Покупка предмета за очки веры
    bool PurchaseFaithItem(uint64_t playerGuid, uint32_t itemId);

    // Обновление пула клонов (раз в 24 часа)
    void RefreshClonePool();

    // Получение очков веры игрока
    uint32_t GetFaithPoints(uint64_t playerGuid) const;

    // Проверка дневного лимита
    bool CheckDailyLimit(uint64_t playerGuid) const;

    // Получение истории матчей
    std::vector<AutoPvPMatch> GetMatchHistory(uint64_t playerGuid, uint32_t limit) const;

private:
    AutoPvPMgr() = default;
    std::vector<PlayerCloneData> m_clonePool;
    std::unordered_map<uint64_t, FaithPoints> m_faithPoints;
    std::vector<AutoPvPMatch> m_matchHistory;
};

} // namespace PvP
} // namespace DM