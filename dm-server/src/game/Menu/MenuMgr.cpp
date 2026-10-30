-- Menu Manager Implementation
-- This file will be created by the build system

#include "game/Menu/MenuMgr.h"
#include "common/Logger.h"

MenuMgr::MenuMgr() : m_menuSystemEnabled(true) {}

MenuMgr::~MenuMgr() {}

void MenuMgr::Initialize() {
    LoadMenus();
    Debug("Menu system initialized");
}

void MenuMgr::Shutdown() {
    // Save all data
    Debug("Menu system shutdown");
}

bool MenuMgr::GetMenuDefinition(uint32 menuId, MenuDef& menuDef) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_menus.find(menuId);
    if (it == m_menus.end()) return false;
    
    menuDef = it->second;
    return true;
}

std::vector<MenuDef> MenuMgr::GetMainMenu() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<MenuDef> mainMenus;
    for (const auto& pair : m_menus) {
        if (pair.second.parent_menu_id == 0 && pair.second.is_active) {
            mainMenus.push_back(pair.second);
        }
    }
    
    // Sort by sort_order
    return mainMenus;
}

std::vector<MenuDef> MenuMgr::GetSubMenu(uint32 parentId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<MenuDef> subMenus;
    for (const auto& pair : m_menus) {
        if (pair.second.parent_menu_id == parentId && pair.second.is_active) {
            subMenus.push_back(pair.second);
        }
    }
    
    // Sort by sort_order
    return subMenus;
}

bool MenuMgr::IsMenuActive(uint32 menuId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_menus.find(menuId);
    if (it == m_menus.end()) return false;
    
    return it->second.is_active;
}

bool MenuMgr::GetPlayerProfile(uint64 playerGuid, PlayerProfile& profile) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_profiles.find(playerGuid);
    if (it == m_profiles.end()) return false;
    
    profile = it->second;
    return true;
}

bool MenuMgr::UpdatePlayerProfile(uint64 playerGuid, const PlayerProfile& profile) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_profiles[playerGuid] = profile;
    
    // Save to database
    SaveProfile(playerGuid, profile);
    
    return true;
}

bool MenuMgr::CreatePlayerProfile(uint64 playerGuid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if profile already exists
    if (m_profiles.find(playerGuid) != m_profiles.end()) return false;
    
    PlayerProfile newProfile;
    newProfile.player_guid = playerGuid;
    newProfile.race_id = 1;
    newProfile.class_id = 1;
    newProfile.level = 1;
    newProfile.experience = 0;
    newProfile.next_level_experience = 100;
    newProfile.strength = 10;
    newProfile.agility = 10;
    newProfile.intelligence = 10;
    newProfile.stamina = 10;
    newProfile.spirit = 100;
    newProfile.gold = 0;
    newProfile.faction_id = 1;
    newProfile.title_id = 0;
    newProfile.hair_style = "";
    newProfile.hair_color = "";
    newProfile.skin_color = "";
    newProfile.creation_time = (uint32)time(nullptr);
    newProfile.last_login = 0;
    newProfile.total_play_time = 0;
    
    m_profiles[playerGuid] = newProfile;
    
    // Save to database
    SaveProfile(playerGuid, newProfile);
    
    return true;
}

bool MenuMgr::IsMenuSystemEnabled() const {
    return m_menuSystemEnabled;
}

uint32 MenuMgr::GetMenuConfigUint(const std::string& configKey, uint32 defaultValue) const {
    // Would query database or config
    // For now return default
    (void)configKey;
    return defaultValue;
}

std::string MenuMgr::GetMenuConfigString(const std::string& configKey, const std::string& defaultValue) const {
    // Would query database or config
    // For now return default
    (void)configKey;
    return defaultValue;
}