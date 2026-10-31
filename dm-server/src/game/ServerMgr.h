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
    
    // Enhanced NPC Management Functions
    bool scheduleNpcRespawn(int npc_id, int respawn_time_seconds);
    bool cancelNpcRespawn(int npc_id);
    bool isNpcRespawnScheduled(int npc_id);
    
    bool updateNpcAI(int npc_id, float delta_time);
    std::string getNpcAIBehavior(int npc_id);
    bool setNpcAIBehavior(int npc_id, const std::string& behavior_type);
    
    bool setNpcReputation(int npc_id, int faction_id, int reputation_value);
    int getNpcReputation(int npc_id, int faction_id);
    std::vector<std::pair<int, int>> getNpcReputationHistory(int npc_id);
    
    bool spawnNpcIfConditionsMet(int npc_id, int player_level, int player_faction);
    bool checkNpcSpawnConditions(int npc_id, int player_level, int player_faction);
    
    std::vector<NpcInfo> getNpcGroup(int npc_id);
    bool addNpcToGroup(int leader_npc_id, int follower_npc_id);
    void removeNpcFromGroup(int npc_id);
    
    NpcInfo scaleNpcLevel(int npc_id, int player_level);
    
    #define MAX_INVENTORY_SIZE 60

// Inventory Slot Structure
struct InventorySlot {
    int item_id; // 0 = empty
    int item_count; // stack count
    int item_quality; // 0=common, 1=rare, 2=epic, 3=legendary
    int durability; // current durability
    int max_durability; // maximum durability
    int enchant_level; // enchantment level
    bool is_equipped; // is item equipped
    int equip_slot; // equipment slot if equipped
};

// Inventory Structure
struct PlayerInventory {
    InventorySlot slots[MAX_INVENTORY_SIZE];
    uint64_t player_guid;
    uint32_t gold;
    uint32_t spirit;
    
    PlayerInventory() {
        // Initialize all slots to empty
        for (int i = 0; i < MAX_INVENTORY_SIZE; i++) {
            slots[i].item_id = 0;
            slots[i].item_count = 0;
            slots[i].item_quality = 0;
            slots[i].durability = 0;
            slots[i].max_durability = 0;
            slots[i].enchant_level = 0;
            slots[i].is_equipped = false;
            slots[i].equip_slot = -1;
        }
        gold = 0;
        spirit = 0;
    }
};

// Inventory Functions
struct InventorySlot getInventorySlot(uint64_t player_guid, int slot_index);
bool setInventorySlot(uint64_t player_guid, int slot_index, const InventorySlot& slot);
bool addItemToInventory(uint64_t player_guid, int item_id, int count, int quality, int durability);
bool removeItemFromInventory(uint64_t player_guid, int item_id, int count);
bool hasItemInInventory(uint64_t player_guid, int item_id, int count = 1);
bool updateInventoryDurability(uint64_t player_guid, int item_id, int durability);

// Inventory Equip/Unequip
bool equipItem(uint64_t player_guid, int slot_index);
bool unequipItem(uint64_t player_guid, int equip_slot);

// Inventory Gold/Spirit Management
bool addGold(uint64_t player_guid, uint32_t amount);
bool removeGold(uint64_t player_guid, uint32_t amount);
uint32_t getGold(uint64_t player_guid);
bool addSpirit(uint64_t player_guid, uint32_t amount);
bool removeSpirit(uint64_t player_guid, uint32_t amount);
uint32_t getSpirit(uint64_t player_guid);

// Inventory Persistence
bool saveInventory(uint64_t player_guid);
bool loadInventory(uint64_t player_guid);

// Inventory Query Functions
std::vector<InventorySlot> getAllInventoryItems(uint64_t player_guid);
std::vector<InventorySlot> getEquippedItems(uint64_t player_guid);
std::vector<InventorySlot> getUsableItems(uint64_t player_guid, int player_level);

// Inventory Search Functions
int findItemInInventory(uint64_t player_guid, int item_id);
std::vector<int> findItemsByQuality(uint64_t player_guid, int quality);
std::vector<int> findItemsByType(uint64_t player_guid, int item_type);

// Inventory Swap
bool swapInventorySlots(uint64_t player_guid, int slot1, int slot2);

// Inventory Filter Functions
std::vector<InventorySlot> filterInventoryByLevel(uint64_t player_guid, int min_level, int max_level);
std::vector<InventorySlot> filterInventoryByType(uint64_t player_guid, int item_type);
std::vector<InventorySlot> filterInventoryByQuality(uint64_t player_guid, int min_quality, int max_quality);

// Inventory Weight System (if implemented)
struct ItemWeightInfo {
    int item_id;
    int weight;
};
float getTotalInventoryWeight(uint64_t player_guid);
float getMaxCarryWeight(uint64_t player_guid, int strength);
float calculateEncumbrance(uint64_t player_guid);

// Inventory Sorting
void sortInventory(uint64_t player_guid, bool by_quality = false, bool by_count = false);
    
    // Server Controls
    bool startCampaign(int campaign_id);
    bool stopCampaign(int campaign_id);
    bool setTimeScale(float scale);
    bool broadcastMessage(const std::string& message, int channel); // 0=global, 1=faction, 2=party, 3=whisper
    
    // Server Settings
    bool setServerSetting(const std::string& key, const std::string& value);
    std::string getServerSetting(const std::string& key);
    std::vector<std::pair<std::string, std::string>> getAllServerSettings();
    
    // Client Settings
    bool setClientSetting(uint64_t player_guid, const std::string& key, const std::string& value);
    std::string getClientSetting(uint64_t player_guid, const std::string& key);
    std::vector<std::pair<std::string, std::string>> getAllClientSettings(uint64_t player_guid);
    
    // Database Settings (admin only)
    bool setDatabaseSetting(const std::string& key, const std::string& value);
    std::string getDatabaseSetting(const std::string& key);
    std::vector<std::pair<std::string, std::string>> getAllDatabaseSettings();
    
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