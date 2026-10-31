#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct ContaminationZone {
    uint32 id;
    std::string name_ru;
    std::string name_en;
    float location_x, location_y, location_z;
    float radius;
    float contamination_level; // 0-100
    bool is_active;
    uint32 spawn_chance; // %
    uint32 respawn_time; // seconds
};

struct PlayerContamination {
    uint64 player_guid;
    uint32 zone_id;
    float infection_level; // 0-100
    uint32 start_time;
    uint32 last_update;
    bool is_cured;
    uint32 cure_time;
};

class ContaminationMgr {
public:
    ContaminationMgr();
    ~ContaminationMgr();

    void Initialize();
    void Shutdown();

    // Zone management
    bool GetContaminationZone(uint32 zone_id, ContaminationZone& zone);
    std::vector<ContaminationZone> GetActiveZones() const;
    bool IsPlayerInContaminationZone(uint64 player_guid, uint32& zone_id) const;
    
    // Player infection state
    bool GetPlayerContamination(uint64 player_guid, uint32 zone_id, PlayerContamination& state);
    bool SetPlayerContamination(uint64 player_guid, uint32 zone_id, float infection_level, uint32 time);
    bool CurePlayerContamination(uint64 player_guid, uint32 zone_id);
    
    // Infection ticks
    void UpdateContamination(uint32 current_time);
    void UpdatePlayerContamination(uint64 player_guid, uint32 current_time);
    
    // Configuration
    bool IsContaminationEnabled() const;
    uint32 GetContaminationTickInterval() const;

private:
    std::map<uint32, ContaminationZone> m_zones;
    std::map<std::pair<uint64, uint32>, PlayerContamination> m_player_contamination;
    bool m_contamination_enabled;
    uint32 m_tick_interval;
    std::mutex m_mutex;

    void LoadZones();
    void LoadConfiguration();
    void SavePlayerContamination(uint64 player_guid, uint32 zone_id, const PlayerContamination& state);
};