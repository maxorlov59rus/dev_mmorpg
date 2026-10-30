-- Campaign Manager Implementation
-- This file will be created by the build system

#include "game/Campaigns/CampaignMgr.h"
#include "common/Logger.h"

CampaignMgr::CampaignMgr() : m_campaigns_enabled(true), m_reset_cooldown(86400) {}

CampaignMgr::~CampaignMgr() {}

void CampaignMgr::Initialize() {
    LoadCampaigns();
    LoadQuests();
    Debug("Campaign system initialized");
}

void CampaignMgr::Shutdown() {
    // Save all data
    Debug("Campaign system shutdown");
}

bool CampaignMgr::GetCampaignDef(uint32 campaign_id, CampaignDef& def) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_campaigns.find(campaign_id);
    if (it == m_campaigns.end()) return false;
    
    def = it->second;
    return true;
}

std::vector<CampaignDef> CampaignMgr::GetAllCampaigns() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<CampaignDef> result;
    for (const auto& pair : m_campaigns) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<CampaignDef> CampaignMgr::GetActiveCampaigns() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<CampaignDef> result;
    for (const auto& pair : m_campaigns) {
        if (pair.second.is_active) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool CampaignMgr::GetCampaignQuest(uint32 campaign_id, uint32 quest_id, CampaignQuest& quest) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto key = std::make_pair(campaign_id, quest_id);
    auto it = m_campaign_quests.find(key);
    if (it == m_campaign_quests.end()) return false;
    
    quest = it->second;
    return true;
}

std::vector<CampaignQuest> CampaignMgr::GetCampaignQuests(uint32 campaign_id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<CampaignQuest> result;
    for (const auto& pair : m_campaign_quests) {
        if (pair.first.first == campaign_id) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool CampaignMgr::GetPlayerProgress(uint64 player_guid, uint32 campaign_id, CampaignProgress& progress) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto key = std::make_pair(player_guid, campaign_id);
    auto it = m_player_campaigns.find(key);
    if (it == m_player_campaigns.end()) return false;
    
    progress = it->second;
    return true;
}

bool CampaignMgr::UpdatePlayerProgress(uint64 player_guid, uint32 campaign_id, const CampaignProgress& progress) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_player_campaigns[std::make_pair(player_guid, campaign_id)] = progress;
    
    // Save to database
    SaveProgress(player_guid, campaign_id, progress);
    
    return true;
}

bool CampaignMgr::AddQuestProgress(uint64 player_guid, uint32 campaign_id, uint32 quest_id, bool completed) {
    CampaignProgress progress;
    if (!GetPlayerProgress(player_guid, campaign_id, progress)) {
        // Create new progress
        progress.player_guid = player_guid;
        progress.campaign_id = campaign_id;
        progress.quests_completed = "[]";
        progress.total_damage_dealt = 0;
        progress.total_damage_taken = 0;
        progress.kills = 0;
        progress.deaths = 0;
        progress.last_activity = (uint32)time(nullptr);
    }
    
    // Update quests_completed JSON
    // This is simplified - real implementation would parse/merge JSON
    progress.last_activity = (uint32)time(nullptr);
    
    return UpdatePlayerProgress(player_guid, campaign_id, progress);
}

bool CampaignMgr::GiveCampaignReward(uint64 player_guid, uint32 campaign_id, uint32 quest_id, uint32& gold_given, uint32& spirit_given) {
    gold_given = 0;
    spirit_given = 0;
    
    // Look up reward config
    // Would query campaign_rewards table
    // For now, give fixed rewards based on campaign difficulty
    
    // Look up campaign def
    CampaignDef def;
    if (!GetCampaignDef(campaign_id, def)) return false;
    
    // Calculate rewards based on difficulty
    uint32 base_gold = 100;
    uint32 base_spirit = 50;
    
    // Scale by campaign id or difficulty
    if (def.difficulty >= 4) {
        base_gold = 500;
        base_spirit = 200;
    } else if (def.difficulty >= 3) {
        base_gold = 300;
        base_spirit = 150;
    }
    
    gold_given = base_gold;
    spirit_given = base_spirit;
    
    // Actually give rewards to player
    // Would call player->AddGold() and player->AddSpirit()
    
    return true;
}

bool CampaignMgr::IsCampaignsEnabled() const {
    return m_campaigns_enabled;
}

uint32 CampaignMgr::GetCampaignResetCooldown() const {
    return m_reset_cooldown;
}