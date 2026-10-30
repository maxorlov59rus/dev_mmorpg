#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "DatabaseEnv.h"
#include "Player.h"

struct MenuDef {
    uint32 id;
    std::string name_ru;
    std::string name_en;
    std::string menu_type; // 'main', 'pause', 'inventory', 'skills', 'quests', 'social', 'settings', 'character'
    uint32 parent_menu_id;
    std::string icon_path;
    bool is_active;
    uint32 sort_order;
};

struct PlayerProfile {
    uint64 player_guid;
    uint32 race_id;
    uint32 class_id;
    uint32 level;
    uint32 experience;
    uint32 next_level_experience;
    uint32 strength;
    uint32 agility;
    uint32 intelligence;
    uint32 stamina;
    uint32 spirit;
    uint32 gold;
    uint32 faction_id;
    uint32 title_id;
    std::string hair_style;
    std::string hair_color;
    std::string skin_color;
    uint32 creation_time;
    uint32 last_login;
    uint32 total_play_time;
};

class MenuMgr {
public:
    MenuMgr();
    ~MenuMgr();

    void Initialize();
    void Shutdown();

    // Menu management
    bool GetMenuDefinition(uint32 menuId, MenuDef& menuDef);
    std::vector<MenuDef> GetMainMenu() const;
    std::vector<MenuDef> GetSubMenu(uint32 parentId) const;
    bool IsMenuActive(uint32 menuId) const;

    // Player profile management
    bool GetPlayerProfile(uint64 playerGuid, PlayerProfile& profile);
    bool UpdatePlayerProfile(uint64 playerGuid, const PlayerProfile& profile);
    bool CreatePlayerProfile(uint64 playerGuid);

    // Configuration
    bool IsMenuSystemEnabled() const;
    uint32 GetMenuConfigUint(const std::string& configKey, uint32 defaultValue = 0) const;
    std::string GetMenuConfigString(const std::string& configKey, const std::string& defaultValue = "") const;

private:
    std::map<uint32, MenuDef> m_menus;
    std::map<uint64, PlayerProfile> m_profiles;
    bool m_menuSystemEnabled;
    std::mutex m_mutex;

    void LoadMenus();
    void LoadProfiles(uint64 playerGuid);
    void SaveProfile(uint64 playerGuid, const PlayerProfile& profile);
};