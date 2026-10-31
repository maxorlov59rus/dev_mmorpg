#ifndef SERVER_MGR_H
#define SERVER_MGR_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

namespace Server {

// Server configuration structure
struct ServerConfig {
    std::string host = "127.0.0.1";
    uint16_t port = 3306;
    std::string username = "mmorpg_admin";
    std::string password = "";
    std::string database = "dm_world";
    uint32_t max_players = 1000;
    uint32_t port_game = 7777;
    bool enable_contamination = true;
    bool enable_campaigns = true;
    bool enable_chat = true;
};

// Server status structure
struct ServerStatus {
    bool running = false;
    uint32_t connected_players = 0;
    uint32_t max_players = 1000;
    uint32_t uptime_seconds = 0;
    double cpu_usage = 0.0;
    double memory_usage_mb = 0.0;
    std::string map_name = "default";
    uint32_t time_scale = 1; // 1=normal, 2=double, 0.5=half
};

// NPC definition structure
struct NpcInfo {
    int id;
    std::string name_ru;
    std::string name_en;
    int npc_type; // 0=monster, 1=quest_giver, 2=merchant, 3=trainer, 4=special
    int rank; // 0=common, 1=rare, 2=epic, 3=boss
    int level;
    int faction_id;
    int city_id;
    float x, y, z; // coordinates
    float spawn_respawn_time; // seconds
    std::string description_ru;
    std::string description_en;
    bool is_active = true;
};

// Quest Giver info
struct QuestGiverInfo {
    int npc_id;
    std::string quest_title_ru;
    std::string quest_title_en;
    int min_level;
    int max_level;
    int reward_gold;
    int reward_spirit;
    int reward_item_id;
    bool is_repeatable;
};

// NPC Quest Data
struct NpcQuestData {
    int npc_id;
    int quest_id;
    bool is_available;
    bool is_completed;
    int progress; // 0 = not started, 1 = in progress, 2 = completed
};

// Quest Reward Data
struct QuestReward {
    int gold;
    int spirit;
    int item_entry;
    int count;
    bool is_experience;
};

// Server Manager Class
class ServerMgr {
public:
    ServerMgr();
    ~ServerMgr();
    
    // Initialization
    bool init(const ServerConfig& config);
    void shutdown();
    
    // Status
    ServerStatus getStatus() const;
    bool isRunning() const { return status_.running; }
    
    // Player Management
    bool addPlayer(uint64_t guid, const std::string& name, int level, int faction_id);
    bool removePlayer(uint64_t guid);
    bool updatePlayerStatus(uint64_t guid, int level, int map_id, float x, float y, float z);
    
    // NPC Management
    std::vector<NpcInfo> getAllNpcs() const;
    std::vector<NpcInfo> getNpcsByFaction(int faction_id) const;
    std::vector<NpcInfo> getNpcsByCity(int city_id) const;
    std::optional<NpcInfo> getNpcById(int npc_id) const;
    bool spawnNpc(const NpcInfo& npc);
    bool despawnNpc(int npc_id);
    bool updateNpcSpawn(int npc_id, float x, float y, float z);
    
    // Quest Giver Management
    std::optional<QuestGiverInfo> getQuestGiver(int npc_id) const;
    std::vector<QuestGiverInfo> getQuestGiversByFaction(int faction_id) const;
    std::vector<QuestGiverInfo> getAvailableQuests(int player_level, int player_faction) const;
    
    // Server Controls
    bool startCampaign(int campaign_id);
    bool stopCampaign(int campaign_id);
    bool setTimeScale(float scale);
    bool broadcastMessage(const std::string& message, int channel); // 0=global, 1=faction, 2=party, 3=whisper
    
    // Database Operations
    bool loadNpcsFromDatabase();
    bool loadQuestsFromDatabase();
    bool savePlayerData(uint64_t guid);
    bool deletePlayerData(uint64_t guid);
    
private:
    ServerConfig config_;
    ServerStatus status_;
    std::mutex data_mutex_;
    std::unique_ptr<sql::Connection> db_conn_;
    
    // Internal methods
    bool connectToDatabase();
    void disconnectFromDatabase();
    void updateUptime();
};
} // namespace Server

#endif // SERVER_MGR_H