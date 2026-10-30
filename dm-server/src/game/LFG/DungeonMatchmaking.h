// DungeonMatchmaking.h — Система подбора в данжи и рейды
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace DM {
namespace LFG {

// Роли в группе
enum LfgRole : uint8_t {
    LFG_ROLE_NONE = 0,
    LFG_ROLE_TANK = 1,
    LFG_ROLE_HEALER = 2,
    LFG_ROLE_DPS = 4,
};

// Сложность данжа
enum DungeonDifficulty : uint8_t {
    DUNGEON_DIFF_NORMAL = 0,
    DUNGEON_DIFF_HEROIC = 1,
    DUNGEON_DIFF_EPIC = 2,
    DUNGEON_DIFF_LEGENDARY = 3,
};

// Статус группы
enum LfgGroupStatus : uint8_t {
    LFG_STATUS_FORMING = 0,
    LFG_STATUS_READY = 1,
    LFG_STATUS_IN_PROGRESS = 2,
    LFG_STATUS_COMPLETED = 3,
};

// Информация о данже
struct DungeonInfo {
    uint16_t id;
    const char* nameRu;
    const char* nameEn;
    uint16_t mapId;
    uint8_t minLevel;
    uint8_t maxLevel;
    uint8_t minPlayers;
    uint8_t maxPlayers;
    bool isRaid;
    uint32_t resetHours;
};

inline DungeonInfo GetDungeonInfo(uint16_t dungeonId) {
    switch (dungeonId) {
        case 20: return {20, "Пещеры Испытаний", "Trial Caves", 20, 10, 15, 1, 5, false, 24};
        case 7:  return {7, "Храм забвения", "Temple of Oblivion", 7, 19, 22, 1, 5, false, 24};
        case 9:  return {9, "Цитадель тьмы", "Dark Citadel", 9, 23, 26, 1, 5, false, 24};
        case 11: return {11, "Разлом Шип", "Rift of Spire", 11, 27, 31, 1, 5, false, 24};
        case 12: return {12, "Цитадель Искажённых", "Citadel of Distorted", 12, 32, 36, 1, 5, false, 24};
        case 13: return {13, "Мёртвые Пределы", "Dead Limits", 13, 37, 41, 1, 5, false, 24};
        case 14: return {14, "Руины Древних", "Ancient Ruins", 14, 42, 46, 1, 5, false, 24};
        case 15: return {15, "Глотка", "The Maw", 15, 47, 50, 10, 10, true, 168};
        case 100: return {100, "Катакомбы Новичков", "Novice Catacombs", 100, 1, 9, 1, 3, false, 12};
        case 101: return {101, "Логово Волков", "Wolf Den", 101, 5, 10, 1, 3, false, 12};
        default: return {0, "Unknown", "Unknown", 0, 0, 0, 0, 0, false, 0};
    }
}

// Информация о сложности
struct DungeonDifficultyInfo {
    uint8_t id;
    const char* nameRu;
    const char* nameEn;
    float hpMultiplier;
    float dmgMultiplier;
    float lootMultiplier;
    float xpMultiplier;
    uint8_t minItemLevel;
};

inline DungeonDifficultyInfo GetDungeonDifficultyInfo(uint8_t diffId) {
    switch (diffId) {
        case DUNGEON_DIFF_NORMAL:
            return {0, "Обычный", "Normal", 1.0f, 1.0f, 1.0f, 1.0f, 0};
        case DUNGEON_DIFF_HEROIC:
            return {1, "Героический", "Heroic", 2.0f, 1.5f, 1.5f, 1.2f, 30};
        case DUNGEON_DIFF_EPIC:
            return {2, "Эпический", "Epic", 4.0f, 2.0f, 2.0f, 1.5f, 50};
        case DUNGEON_DIFF_LEGENDARY:
            return {3, "Легендарный", "Legendary", 8.0f, 3.0f, 3.0f, 2.0f, 70};
        default:
            return {0, "Обычный", "Normal", 1.0f, 1.0f, 1.0f, 1.0f, 0};
    }
}

// Запись в очереди LFG
struct LfgQueueEntry {
    uint64_t playerGuid = 0;
    uint16_t dungeonId = 0;
    uint8_t difficultyId = 0;
    uint8_t roleMask = 0;
    uint64_t queueTime = 0;
    bool isActive = true;
};

// Группа LFG
struct LfgGroup {
    uint64_t groupId = 0;
    uint16_t dungeonId = 0;
    uint8_t difficultyId = 0;
    uint64_t leaderGuid = 0;
    uint64_t createdAt = 0;
    LfgGroupStatus status = LFG_STATUS_FORMING;
    std::vector<uint64_t> memberGuids;
    std::vector<uint8_t> memberRoles;
};

// Функции
class LfgMgr {
public:
    static LfgMgr &Instance();

    // Присоединиться к очереди
    bool JoinQueue(uint64_t playerGuid, uint16_t dungeonId, uint8_t difficultyId, uint8_t roleMask);

    // Покинуть очередь
    bool LeaveQueue(uint64_t playerGuid);

    // Проверка возможности присоединиться
    bool CanJoinQueue(uint64_t playerGuid, uint16_t dungeonId, uint8_t difficultyId) const;

    // Формирование группы (автоматическое)
    bool TryFormGroup(uint16_t dungeonId, uint8_t difficultyId);

    // Проверка состава группы (танк + хил + ДПС)
    bool ValidateGroupComposition(const LfgGroup &group) const;

    // Запуск данжа
    bool StartDungeon(uint64_t groupId);

    // Завершение данжа
    void CompleteDungeon(uint64_t groupId);

    // Сброс данжа
    void ResetDungeon(uint64_t groupId);

    // Получение группы игрока
    LfgGroup* GetPlayerGroup(uint64_t playerGuid);

    // Обновление (таймеры, формирование групп)
    void Update(uint32_t deltaMs);

    // Конфигурация
    uint32_t queueTimeoutSec = 300;
    uint32_t groupFormingTimeoutSec = 60;
    bool autoAccept = true;
    bool difficultyScaling = true;
    uint32_t raidLockoutHours = 168;
    uint32_t dungeonLockoutHours = 24;
    uint8_t minTankPerGroup = 1;
    uint8_t minHealerPerGroup = 1;
    uint8_t minDpsPerGroup = 3;

private:
    LfgMgr() = default;
    std::vector<LfgQueueEntry> m_queue;
    std::vector<LfgGroup> m_groups;
};

} // namespace LFG
} // namespace DM