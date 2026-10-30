#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct AchievementDef {
    uint32 id;
    std::string name_ru;
    std::string name_en;
    std::string description_ru;
    std::string description_en;
    std::string category;
    std::string condition_type;
    std::string condition_value;
    std::string reward_type;
    std::string reward_value;
    bool is_hidden;
    bool is_daily;
    uint32 sort_order;
};

struct AchievementProgress {
    uint64 playerGuid;
    uint32 achievementId;
    uint32 progress;
    bool completed;
    uint32 completedAt;
    uint32 lastUpdate;
};

class AchievementMgr {
public:
    AchievementMgr();
    ~AchievementMgr();

    void Initialize();
    void Shutdown();

    // Check and update achievement progress
    void UpdatePlayerProgress(uint64 playerGuid, const std::string& conditionType, const std::string& conditionValue);
    void CompleteAchievement(uint64 playerGuid, uint32 achievementId);
    void ClaimReward(uint64 playerGuid, uint32 achievementId);

    // Get achievement info
    const AchievementDef* GetAchievementDef(uint32 id) const;
    std::vector<AchievementDef> GetAllAchievements() const;
    std::vector<AchievementProgress> GetPlayerProgress(uint64 playerGuid) const;

    // Faction war methods
    void UpdateFactionWar(uint32 factionA, uint32 factionB, uint32 warState, uint32 reason);
    void AddFactionContribution(uint64 playerGuid, uint32 factionId, uint32 contribution);
    uint32 GetFactionReputation(uint64 playerGuid, uint32 factionId) const;

    // Shop methods
    bool BuyFromShop(uint64 playerGuid, uint32 shopId, uint32& costPoints);
    std::vector<std::pair<std::string, uint32>> GetShopItems(uint64 playerGuid) const;

private:
    std::map<uint32, AchievementDef> m_achievementDefs;
    std::mutex m_mutex;

    void LoadAchievements();
    void LoadPlayerProgress(uint64 playerGuid);
    void SaveProgress(uint64 playerGuid, uint32 achievementId, uint32 progress, bool completed);
};