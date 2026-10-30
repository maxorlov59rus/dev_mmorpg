#include "game/Icons/ExtendedIconMgr.h"
#include <unordered_map>
#include <vector>

namespace dm {

static std::unordered_map<uint32_t, ExtendedIcon> s_icons;
static std::unordered_map<std::string, uint32_t> s_iconByName;

ExtendedIconMgr &ExtendedIconMgr::Instance() {
    static ExtendedIconMgr inst;
    return inst;
}

const ExtendedIcon *ExtendedIconMgr::GetIcon(uint32_t id) const {
    auto it = s_icons.find(id);
    if (it == s_icons.end()) return nullptr;
    return &it->second;
}

const ExtendedIcon *ExtendedIconMgr::GetIconByName(const std::string &name) const {
    auto it = s_iconByName.find(name);
    if (it == s_iconByName.end()) return nullptr;
    return GetIcon(it->second);
}

std::vector<ExtendedIcon> ExtendedIconMgr::GetIconsByCategory(const std::string &category) const {
    std::vector<ExtendedIcon> result;
    for (const auto &kv : s_icons) {
        if (kv.second.category == category) result.push_back(kv.second);
    }
    return result;
}

std::vector<ExtendedIcon> ExtendedIconMgr::GetIconsBySubcategory(const std::string &subcategory) const {
    std::vector<ExtendedIcon> result;
    for (const auto &kv : s_icons) {
        if (kv.second.subcategory == subcategory) result.push_back(kv.second);
    }
    return result;
}

bool ExtendedIconMgr::HasIcon(uint32_t id) const {
    return s_icons.count(id) > 0;
}

void ExtendedIconMgr::LoadAllIcons() {
    // Skills
    const char* skillNames[] = {"shield_bash","taunt","stone_skin","heal","blessing","purify","fireball","frost_bolt","chain_lightning","battle_cry","slow","party_shield","backstab","shadow_step","aimed_shot","multishot","shadow_bolt","drain_life"};
    const char* skillRu[] = {"Удар щитом","Провокация","Каменная кожа","Исцеление","Благословение","Очищение","Огненный шар","Ледяная стрела","Цепная молния","Боевой клич","Замедление","Щит группы","Удар в спину","Шаг тени","Прицельный выстрел","Мульти-выстрел","Стрела тьмы","Вытягивание жизни"};
    const char* skillEn[] = {"Shield Bash","Taunt","Stone Skin","Heal","Blessing","Purify","Fireball","Frost Bolt","Chain Lightning","Battle Cry","Slow","Party Shield","Backstab","Shadow Step","Aimed Shot","Multishot","Shadow Bolt","Drain Life"};
    for (int i = 0; i < 18; i++) {
        ExtendedIcon icon;
        icon.id = i + 1;
        icon.name = std::string("icon_skill_") + skillNames[i];
        icon.filePath = std::string("Icons/Skills/") + skillNames[i];
        icon.category = "skill";
        icon.subcategory = "active";
        icon.sizePx = 64;
        icon.quality = 0;
        icon.descriptionRu = skillRu[i];
        icon.descriptionEn = skillEn[i];
        s_icons[icon.id] = icon;
        s_iconByName[icon.name] = icon.id;
    }
    // Passives
    const char* passNames[] = {"toughness","strength","agility","intellect","vitality","spirit","critical","dodge","block","regeneration"};
    const char* passRu[] = {"Стойкость","Сила","Ловкость","Интеллект","Выносливость","Дух","Критический удар","Уклонение","Блок","Регенерация"};
    const char* passEn[] = {"Toughness","Strength","Agility","Intellect","Vitality","Spirit","Critical Strike","Dodge","Block","Regeneration"};
    for (int i = 0; i < 10; i++) {
        ExtendedIcon icon;
        icon.id = 19 + i;
        icon.name = std::string("icon_passive_") + passNames[i];
        icon.filePath = std::string("Icons/Passive/") + passNames[i];
        icon.category = "passive";
        icon.subcategory = "passive";
        icon.sizePx = 64;
        icon.quality = 0;
        icon.descriptionRu = passRu[i];
        icon.descriptionEn = passEn[i];
        s_icons[icon.id] = icon;
        s_iconByName[icon.name] = icon.id;
    }
    // Weapons
    const char* weapNames[] = {"sword_iron","sword_steel","sword_mithril","sword_titanium","shield_iron","shield_steel","shield_mithril","shield_titanium","dagger","bow","staff","wand"};
    const char* weapRu[] = {"Железный меч","Стальной меч","Мифриловый меч","Титановый меч","Железный щит","Стальной щит","Мифриловый щит","Титановый щит","Кинжал","Лук","Посох","Жезл"};
    const char* weapEn[] = {"Iron Sword","Steel Sword","Mithril Sword","Titanium Sword","Iron Shield","Steel Shield","Mithril Shield","Titanium Shield","Dagger","Bow","Staff","Wand"};
    uint8_t weapQ[] = {1,2,3,4,1,2,3,4,2,2,2,2};
    for (int i = 0; i < 12; i++) {
        ExtendedIcon icon;
        icon.id = 29 + i;
        icon.name = std::string("icon_weapon_") + weapNames[i];
        icon.filePath = std::string("Icons/Weapons/") + weapNames[i];
        icon.category = "weapon";
        icon.subcategory = "weapon";
        icon.sizePx = 64;
        icon.quality = weapQ[i];
        icon.descriptionRu = weapRu[i];
        icon.descriptionEn = weapEn[i];
        s_icons[icon.id] = icon;
        s_iconByName[icon.name] = icon.id;
    }
}

bool ExtendedIconMgr::IsEnabled() const { return true; }

} // namespace dm