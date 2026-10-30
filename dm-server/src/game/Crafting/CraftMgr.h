// CraftMgr.h — Система крафта: профессии, рецепты, материалы
#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

namespace dm {

struct Recipe {
    uint32_t recipeId;
    uint32_t professionId;
    std::string nameRu;
    std::string nameEn;
    uint32_t resultItemId;
    uint32_t resultCount;
    uint32_t requiredSkill;
    uint32_t craftTimeMs;
    std::unordered_map<uint32_t, uint32_t> materials;
};

struct Profession {
    uint8_t id;
    std::string nameRu;
    std::string nameEn;
    uint8_t type;
    uint8_t minLevel;
    uint32_t maxSkill;
};

class CraftMgr {
public:
    static CraftMgr &Instance();
    
    bool CanLearnProfession(uint32_t playerGuid, uint8_t professionId, uint8_t playerLevel) const;
    bool LearnProfession(uint32_t playerGuid, uint8_t professionId);
    bool HasProfession(uint32_t playerGuid, uint8_t professionId) const;
    uint32_t GetSkillLevel(uint32_t playerGuid, uint8_t professionId) const;
    void SetSkillLevel(uint32_t playerGuid, uint8_t professionId, uint32_t level);
    uint32_t GetSkillMax(uint32_t playerGuid, uint8_t professionId) const;
    
    bool CanCraft(uint32_t playerGuid, uint32_t recipeId) const;
    bool Craft(uint32_t playerGuid, uint32_t recipeId);
    void LearnRecipe(uint32_t playerGuid, uint32_t recipeId);
    bool KnowsRecipe(uint32_t playerGuid, uint32_t recipeId) const;
    const Recipe *GetRecipe(uint32_t recipeId) const;
    std::vector<uint32_t> GetKnownRecipes(uint32_t playerGuid) const;
    
    uint32_t GetItemCount(uint32_t playerGuid, uint32_t itemId) const;
    void ConsumeMaterial(uint32_t playerGuid, uint32_t itemId, uint32_t count);
    void AddItem(uint32_t playerGuid, uint32_t itemId, uint32_t count);
    void RemoveItem(uint32_t playerGuid, uint32_t itemId, uint32_t count);
    
    bool HasMaterials(uint32_t playerGuid, uint32_t recipeId) const;
    bool HasSkillLevel(uint32_t playerGuid, uint32_t recipeId) const;
    
    uint8_t GetPlayerLevel(uint32_t playerGuid) const;
    void SetPlayerLevel(uint32_t playerGuid, uint8_t level);
    
    void LoadRecipes();
    void LoadProfessions();
};

} // namespace dm