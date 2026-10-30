#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct CampaignDef {
    uint32 id;
    std::string name_ru;
    std::string name_en;
    uint32 level_start;
    uint32 level_end;
    uint32 min_players;
    uint32 max_players;
    uint32 duration_minutes;
    uint8 difficulty; // 1-5
    bool is_active;
    uint32 sort_order;
};

struct CampaignQuest {
    uint32 id;
    uint32 campaign_id;
    std::string name_ru;
    std::string name_en;
    std::string description_ru;
    std::string description_en;
    std::string quest_type; // kill, collect, escort, defend, puzzle, boss
    uint32 min_level;
    uint32 max_level;
    uint32 required_power;
    uint32 reward_gold;
    uint32 reward_spirit;
    uint32 reward_item_entry;
    uint32 required_completed_campaign; // 0 = none, ID если цепочка
    bool is_repeatable;
    uint32 sort_order;
};

struct CampaignProgress {
    uint64 player_guid;
    uint32 campaign_id;
    std::string quests_completed; // JSON string
    uint32 total_damage_dealt;
    uint32 total_damage_taken;
    uint32 kills;
    uint32 deaths;
    uint32 last_activity;
};

class CampaignMgr {
public:
    CampaignMgr();
    ~CampaignMgr();

    void Initialize();
    void Shutdown();

    // Campaign management
    bool GetCampaignDef(uint32 campaign_id, CampaignDef& def);
    std::vector<CampaignDef> GetAllCampaigns() const;
    std::vector<CampaignDef> GetActiveCampaigns() const;
    
    // Quest management
    bool GetCampaignQuest(uint32 campaign_id, uint32 quest_id, CampaignQuest& quest);
    std::vector<CampaignQuest> GetCampaignQuests(uint32 campaign_id) const;
    
    // Player progress
    bool GetPlayerProgress(uint64 player_guid, uint32 campaign_id, CampaignProgress& progress);
    bool UpdatePlayerProgress(uint64 player_guid, uint32 campaign_id, const CampaignProgress& progress);
    bool AddQuestProgress(uint64 player_guid, uint32 campaign_id, uint32 quest_id, bool completed);
    
    // Rewards
    bool GiveCampaignReward(uint64 player_guid, uint32 campaign_id, uint32 quest_id, uint32& gold_given, uint32& spirit_given);
    
    // Configuration
    bool IsCampaignsEnabled() const;
    uint32 GetCampaignResetCooldown() const;

private:
    std::map<uint32, CampaignDef> m_campaigns;
    std::map<std::pair<uint32, uint32>, CampaignQuest> m_campaign_quests; // (campaign_id, quest_id)
    std::map<std::pair<uint64, uint32>, CampaignProgress> m_player_campaigns; // (player_guid, campaign_id)
    bool m_campaigns_enabled;
    uint32 m_reset_cooldown;
    std::mutex m_mutex;

    void LoadCampaigns();
    void LoadQuests();
    void LoadPlayerProgress(uint64 player_guid);
    void SaveProgress(uint64 player_guid, uint32 campaign_id, const CampaignProgress& progress);
};