#include "game/Crafting/CraftMgr.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dm {

static std::unordered_map<uint32_t, std::unordered_set<uint32_t>> s_knownRecipes;
static std::unordered_map<uint32_t, std::unordered_map<uint8_t, uint32_t>> s_skills;
static std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> s_inventory;
static std::unordered_map<uint32_t, uint8_t> s_playerLevel;
static std::unordered_map<uint32_t, std::unordered_set<uint8_t>> s_professions;
static std::unordered_map<uint32_t, Recipe> s_recipes;
static std::unordered_map<uint8_t, Profession> s_professionsDef;

CraftMgr &CraftMgr::Instance() {
    static CraftMgr inst;
    return inst;
}

bool CraftMgr::CanLearnProfession(uint32_t playerGuid, uint8_t professionId, uint8_t playerLevel) const {
    if (playerLevel < 20) return false;
    auto it = s_professionsDef.find(professionId);
    if (it == s_professionsDef.end()) return false;
    if (playerLevel < it->second.minLevel) return false;
    auto pit = s_professions.find(playerGuid);
    if (pit != s_professions.end() && pit->second.size() >= 2) return false;
    return true;
}

bool CraftMgr::LearnProfession(uint32_t playerGuid, uint8_t professionId) {
    s_professions[playerGuid].insert(professionId);
    s_skills[playerGuid][professionId] = 1;
    return true;
}

bool CraftMgr::HasProfession(uint32_t playerGuid, uint8_t professionId) const {
    auto it = s_professions.find(playerGuid);
    if (it == s_professions.end()) return false;
    return it->second.count(professionId) > 0;
}

uint32_t CraftMgr::GetSkillLevel(uint32_t playerGuid, uint8_t professionId) const {
    auto it = s_skills.find(playerGuid);
    if (it == s_skills.end()) return 0;
    auto it2 = it->second.find(professionId);
    if (it2 == it->second.end()) return 0;
    return it2->second;
}

void CraftMgr::SetSkillLevel(uint32_t playerGuid, uint8_t professionId, uint32_t level) {
    s_skills[playerGuid][professionId] = level;
}

uint32_t CraftMgr::GetSkillMax(uint32_t playerGuid, uint8_t professionId) const {
    uint8_t level = GetPlayerLevel(playerGuid);
    return level * 5;
}

bool CraftMgr::CanCraft(uint32_t playerGuid, uint32_t recipeId) const {
    if (!KnowsRecipe(playerGuid, recipeId)) return false;
    if (!HasMaterials(playerGuid, recipeId)) return false;
    if (!HasSkillLevel(playerGuid, recipeId)) return false;
    return true;
}

bool CraftMgr::Craft(uint32_t playerGuid, uint32_t recipeId) {
    if (!CanCraft(playerGuid, recipeId)) return false;
    const Recipe *r = GetRecipe(recipeId);
    if (!r) return false;
    
    // Consume materials
    for (auto &m : r->materials) {
        ConsumeMaterial(playerGuid, m.first, m.second);
    }
    
    // Add result item
    AddItem(playerGuid, r->resultItemId, r->resultCount);
    
    // Gain skill
    uint32_t skill = GetSkillLevel(playerGuid, (uint8_t)r->professionId);
    uint32_t maxSkill = GetSkillMax(playerGuid, (uint8_t)r->professionId);
    if (skill < maxSkill) {
        SetSkillLevel(playerGuid, (uint8_t)r->professionId, skill + 1);
    }
    
    return true;
}

void CraftMgr::LearnRecipe(uint32_t playerGuid, uint32_t recipeId) {
    s_knownRecipes[playerGuid].insert(recipeId);
}

bool CraftMgr::KnowsRecipe(uint32_t playerGuid, uint32_t recipeId) const {
    auto it = s_knownRecipes.find(playerGuid);
    if (it == s_knownRecipes.end()) return false;
    return it->second.count(recipeId) > 0;
}

const Recipe *CraftMgr::GetRecipe(uint32_t recipeId) const {
    auto it = s_recipes.find(recipeId);
    if (it == s_recipes.end()) return nullptr;
    return &it->second;
}

std::vector<uint32_t> CraftMgr::GetKnownRecipes(uint32_t playerGuid) const {
    std::vector<uint32_t> result;
    auto it = s_knownRecipes.find(playerGuid);
    if (it != s_knownRecipes.end()) {
        for (uint32_t rid : it->second) {
            result.push_back(rid);
        }
    }
    return result;
}

uint32_t CraftMgr::GetItemCount(uint32_t playerGuid, uint32_t itemId) const {
    auto it = s_inventory.find(playerGuid);
    if (it == s_inventory.end()) return 0;
    auto it2 = it->second.find(itemId);
    if (it2 == it->second.end()) return 0;
    return it2->second;
}

void CraftMgr::ConsumeMaterial(uint32_t playerGuid, uint32_t itemId, uint32_t count) {
    auto &inv = s_inventory[playerGuid];
    auto it = inv.find(itemId);
    if (it != inv.end()) {
        if (it->second >= count) it->second -= count;
        else it->second = 0;
    }
}

void CraftMgr::AddItem(uint32_t playerGuid, uint32_t itemId, uint32_t count) {
    s_inventory[playerGuid][itemId] += count;
}

void CraftMgr::RemoveItem(uint32_t playerGuid, uint32_t itemId, uint32_t count) {
    auto &inv = s_inventory[playerGuid];
    auto it = inv.find(itemId);
    if (it != inv.end()) {
        if (it->second >= count) it->second -= count;
        else it->second = 0;
    }
}

bool CraftMgr::HasMaterials(uint32_t playerGuid, uint32_t recipeId) const {
    const Recipe *r = GetRecipe(recipeId);
    if (!r) return false;
    for (auto &m : r->materials) {
        if (GetItemCount(playerGuid, m.first) < m.second) return false;
    }
    return true;
}

bool CraftMgr::HasSkillLevel(uint32_t playerGuid, uint32_t recipeId) const {
    const Recipe *r = GetRecipe(recipeId);
    if (!r) return false;
    uint32_t skill = GetSkillLevel(playerGuid, (uint8_t)r->professionId);
    return skill >= r->requiredSkill;
}

uint8_t CraftMgr::GetPlayerLevel(uint32_t playerGuid) const {
    auto it = s_playerLevel.find(playerGuid);
    if (it == s_playerLevel.end()) return 1;
    return it->second;
}

void CraftMgr::SetPlayerLevel(uint32_t playerGuid, uint8_t level) {
    s_playerLevel[playerGuid] = level;
}

void CraftMgr::LoadRecipes() {
    // TODO: Load from database
}

void CraftMgr::LoadProfessions() {
    // TODO: Load from database
}

} // namespace dm