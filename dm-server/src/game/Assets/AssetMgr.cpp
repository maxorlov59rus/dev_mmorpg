#include "game/Assets/AssetMgr.h"
#include <unordered_map>
#include <vector>

namespace dm {

static std::unordered_map<uint32_t, MusicTrack> s_music;
static std::unordered_map<uint32_t, ModelDef> s_models;
static std::unordered_map<uint32_t, IconDef> s_icons;
static std::unordered_map<std::string, uint32_t> s_iconByName;
static std::unordered_map<uint32_t, LoreEntry> s_lore;

AssetMgr &AssetMgr::Instance() {
    static AssetMgr inst;
    return inst;
}

const MusicTrack *AssetMgr::GetMusicTrack(uint32_t id) const {
    auto it = s_music.find(id);
    if (it == s_music.end()) return nullptr;
    return &it->second;
}

std::vector<MusicTrack> AssetMgr::GetMusicByCategory(const std::string &category) const {
    std::vector<MusicTrack> result;
    for (const auto &kv : s_music) {
        if (kv.second.category == category) result.push_back(kv.second);
    }
    return result;
}

bool AssetMgr::HasMusicTrack(uint32_t id) const {
    return s_music.count(id) > 0;
}

const ModelDef *AssetMgr::GetModel(uint32_t id) const {
    auto it = s_models.find(id);
    if (it == s_models.end()) return nullptr;
    return &it->second;
}

std::vector<ModelDef> AssetMgr::GetModelsByType(const std::string &type) const {
    std::vector<ModelDef> result;
    for (const auto &kv : s_models) {
        if (kv.second.modelType == type) result.push_back(kv.second);
    }
    return result;
}

const IconDef *AssetMgr::GetIcon(uint32_t id) const {
    auto it = s_icons.find(id);
    if (it == s_icons.end()) return nullptr;
    return &it->second;
}

const IconDef *AssetMgr::GetIconByName(const std::string &name) const {
    auto it = s_iconByName.find(name);
    if (it == s_iconByName.end()) return nullptr;
    return GetIcon(it->second);
}

const LoreEntry *AssetMgr::GetLore(uint32_t id) const {
    auto it = s_lore.find(id);
    if (it == s_lore.end()) return nullptr;
    return &it->second;
}

std::vector<LoreEntry> AssetMgr::GetLoreByCategory(const std::string &category) const {
    std::vector<LoreEntry> result;
    for (const auto &kv : s_lore) {
        if (kv.second.category == category) result.push_back(kv.second);
    }
    return result;
}

void AssetMgr::LoadAllAssets() {
    // Music tracks
    MusicTrack m1; m1.id=1; m1.nameRu="Тема главного меню"; m1.nameEn="Main Menu Theme"; m1.filePath="Audio/Music/main_menu_theme"; m1.category="ui"; m1.durationSec=180; m1.loopTrack=true; m1.volume=0.8f;
    s_music[1]=m1;
    MusicTrack m2; m2.id=2; m2.nameRu="Амбиент города"; m2.nameEn="City Ambient"; m2.filePath="Audio/Music/city_ambient"; m2.category="ambient"; m2.durationSec=240; m2.loopTrack=true; m2.volume=0.6f;
    s_music[2]=m2;
    MusicTrack m3; m3.id=3; m3.nameRu="Боевая тема"; m3.nameEn="Combat Theme"; m3.filePath="Audio/Music/combat_theme"; m3.category="combat"; m3.durationSec=90; m3.loopTrack=true; m3.volume=0.9f;
    s_music[3]=m3;
    MusicTrack m4; m4.id=4; m4.nameRu="Босс: Цитадель Тьмы"; m4.nameEn="Boss: Citadel"; m4.filePath="Audio/Music/boss_citadel"; m4.category="boss"; m4.durationSec=120; m4.loopTrack=true; m4.volume=1.0f;
    s_music[4]=m4;
    MusicTrack m5; m5.id=5; m5.nameRu="Босс: Логово Дракона"; m5.nameEn="Boss: Dragon Lair"; m5.filePath="Audio/Music/boss_dragon"; m5.category="boss"; m5.durationSec=150; m5.loopTrack=true; m5.volume=1.0f;
    s_music[5]=m5;

    // Models
    ModelDef md1; md1.id=1; md1.nameRu="Воин"; md1.nameEn="Warrior"; md1.modelPath="Models/Characters/warrior"; md1.modelType="character"; md1.polyCount=5000; md1.textureSize=1024;
    s_models[1]=md1;
    ModelDef md2; md2.id=2; md2.nameRu="Жрец"; md2.nameEn="Priest"; md2.modelPath="Models/Characters/priest"; md2.modelType="character"; md2.polyCount=4500; md2.textureSize=1024;
    s_models[2]=md2;
    ModelDef md3; md3.id=3; md3.nameRu="Маг"; md3.nameEn="Mage"; md3.modelPath="Models/Characters/mage"; md3.modelType="character"; md3.polyCount=4800; md3.textureSize=1024;
    s_models[3]=md3;

    // Icons
    IconDef ic1; ic1.id=1; ic1.name="icon_warrior"; ic1.filePath="Icons/Classes/warrior"; ic1.category="class"; ic1.sizePx=64;
    s_icons[1]=ic1; s_iconByName["icon_warrior"]=1;
    IconDef ic2; ic2.id=2; ic2.name="icon_priest"; ic2.filePath="Icons/Classes/priest"; ic2.category="class"; ic2.sizePx=64;
    s_icons[2]=ic2; s_iconByName["icon_priest"]=2;
    IconDef ic3; ic3.id=3; ic3.name="icon_mage"; ic3.filePath="Icons/Classes/mage"; ic3.category="class"; ic3.sizePx=64;
    s_icons[3]=ic3; s_iconByName["icon_mage"]=3;

    // Lore
    LoreEntry l1; l1.id=1; l1.category="world"; l1.entityId=0; l1.titleRu="Сотворение мира"; l1.titleEn="Creation of the World"; l1.textRu="В начале был Хаос."; l1.textEn="In the beginning there was Chaos."; l1.authorRu="Неизвестный"; l1.authorEn="Unknown";
    s_lore[1]=l1;
    LoreEntry l2; l2.id=2; l2.category="faction"; l2.entityId=1; l2.titleRu="Ковенант Рассвета"; l2.titleEn="Covenant of Dawn"; l2.textRu="Фракция света и порядка."; l2.textEn="Faction of light and order."; l2.authorRu="Летописец Веларион"; l2.authorEn="Chronicler Velarion";
    s_lore[2]=l2;
}

bool AssetMgr::IsEnabled() const { return true; }
float AssetMgr::GetMusicVolume() const { return 0.7f; }
float AssetMgr::GetSfxVolume() const { return 0.8f; }

} // namespace dm