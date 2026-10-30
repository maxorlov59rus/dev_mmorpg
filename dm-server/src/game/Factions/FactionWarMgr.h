#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct FactionWar {
    uint32 id;
    uint32 factionA;
    uint32 factionB;
    uint32 warState; // 0=мир, 1=война, 2=перемирие
    uint32 startTime;
    uint32 endTime;
    std::string reason;
    uint32 weeklyScoreA;
    uint32 weeklyScoreB;
};

struct FactionReputation {
    uint64 playerGuid;
    uint32 factionId;
    int32 reputation; // -5000 to 5000
    uint32 contribution;
    uint32 dailyContribution;
    uint32 lastLogin;
};

class FactionWarMgr {
public:
    FactionWarMgr();
    ~FactionWarMgr();

    void Initialize();
    void Shutdown();

    // War state management
    void SetWarState(uint32 factionA, uint32 factionB, uint32 warState, uint32 reason);
    uint32 GetWarState(uint32 factionA, uint32 factionB) const;
    
    // Contribution management
    void AddContribution(uint64 playerGuid, uint32 factionId, uint32 contribution);
    uint32 GetContribution(uint64 playerGuid, uint32 factionId) const;
    
    // Reputation management
    void SetReputation(uint64 playerGuid, uint32 factionId, int32 reputation);
    int32 GetReputation(uint64 playerGuid, uint32 factionId) const;
    
    // War participation
    bool IsPlayerInWar(uint64 playerGuid, uint32 factionId) const;
    
    // Get all active wars
    std::vector<FactionWar> GetActiveWars() const;

private:
    std::map<std::pair<uint32, uint32>, FactionWar> m_wars;
    std::map<std::pair<uint64, uint32>, FactionReputation> m_reputations;
    std::mutex m_mutex;

    void LoadWars();
    void LoadReputations(uint64 playerGuid);
};