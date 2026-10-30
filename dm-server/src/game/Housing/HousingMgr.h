#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct HouseInfo {
    uint64 id;
    uint64 playerGuid;
    std::string houseType;
    float locationX, locationY, locationZ;
    float orientation;
    uint64 landPlotId;
    uint32 status; // 0=built, 1=under_construction, 2=decorating
    uint32 goldCost;
    uint32 constructionTime;
    uint32 lastUpdate;
};

struct FarmInfo {
    uint64 id;
    uint64 playerGuid;
    uint32 farmId;
    std::string farmType;
    float positionX, positionY, positionZ;
    uint8 growthStage; // 0=empty, 1=seedling, 2=growing, 3=ready, 4=harvested
    uint32 harvestCount;
    uint32 lastHarvest;
    bool isActive;
};

struct HouseBuff {
    std::string houseType;
    std::string buffType;
    float buffValue;
    uint32 durationHours;
    bool isActive;
};

class HousingMgr {
public:
    HousingMgr();
    ~HousingMgr();

    void Initialize();
    void Shutdown();

    // House management
    bool GetPlayerHouse(uint64 playerGuid, HouseInfo& houseInfo);
    bool CreateHouse(uint64 playerGuid, const std::string& houseType, float x, float y, float z);
    bool UpdateHouse(uint64 playerGuid, uint32 status, uint32 goldCost);
    bool DeleteHouse(uint64 playerGuid);
    
    // Farm management
    bool GetPlayerFarm(uint64 playerGuid, uint32 farmId, FarmInfo& farmInfo);
    bool CreateFarm(uint64 playerGuid, uint32 farmId, float x, float y, float z);
    bool UpdateFarm(uint64 playerGuid, uint32 farmId, uint8 growthStage, uint32 harvestCount);
    bool HarvestFarm(uint64 playerGuid, uint32 farmId);
    
    // Buffs
    std::vector<HouseBuff> GetHouseBuffs(uint64 playerGuid) const;
    bool ApplyHouseBuffs(uint64 playerGuid) const;
    
    // Configuration
    bool IsHousingEnabled() const;
    uint32 GetMaxHousesPerPlayer() const;

private:
    std::map<uint64, HouseInfo> m_houses;
    std::map<std::pair<uint64, uint32>, FarmInfo> m_farms;
    std::vector<HouseBuff> m_houseBuffs;
    bool m_housingEnabled;
    uint32 m_maxHousesPerPlayer;
    std::mutex m_mutex;

    void LoadHouses();
    void LoadFarms(uint64 playerGuid);
    void LoadBuffs();
    void SaveHouse(uint64 playerGuid, const HouseInfo& houseInfo);
    void SaveFarm(uint64 playerGuid, uint32 farmId, const FarmInfo& farmInfo);
};