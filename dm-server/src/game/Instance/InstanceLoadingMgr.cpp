#include "game/Instance/InstanceLoadingMgr.h"
#include <unordered_map>
#include <vector>

namespace dm {

static std::unordered_map<uint32_t, LoadingScreen> s_loadingScreens;
static LoadingScreenConfig s_config = {
    true, LoadingAnimationType::Fade, 3000, true, true, 500, 500, 2000
};

InstanceLoadingMgr &InstanceLoadingMgr::Instance() {
    static InstanceLoadingMgr inst;
    return inst;
}

const LoadingScreen *InstanceLoadingMgr::GetLoadingScreen(uint32_t mapId) const {
    auto it = s_loadingScreens.find(mapId);
    if (it == s_loadingScreens.end()) return nullptr;
    return &it->second;
}

std::vector<LoadingScreen> InstanceLoadingMgr::GetAllLoadingScreens() const {
    std::vector<LoadingScreen> result;
    for (const auto &kv : s_loadingScreens) {
        result.push_back(kv.second);
    }
    return result;
}

bool InstanceLoadingMgr::HasLoadingScreen(uint32_t mapId) const {
    return s_loadingScreens.count(mapId) > 0;
}

LoadingScreenConfig InstanceLoadingMgr::GetConfig() const {
    return s_config;
}

void InstanceLoadingMgr::SetConfig(const LoadingScreenConfig &config) {
    s_config = config;
}

void InstanceLoadingMgr::LoadLoadingScreens() {
    // Dungeon of the Forgotten
    LoadingScreen ls1;
    ls1.id = 1; ls1.instanceType = InstanceType::Dungeon; ls1.mapId = 2001;
    ls1.nameRu = "Подземелье Забытых"; ls1.nameEn = "Dungeon of the Forgotten";
    ls1.backgroundImage = "bg_dungeon_forgotten"; ls1.animationType = LoadingAnimationType::Fade;
    ls1.animationDurationMs = 3000; ls1.tipRu = "Используйте факелы для освещения тёмных коридоров.";
    ls1.tipEn = "Use torches to light dark corridors."; ls1.musicTrack = "music_dungeon_ambient";
    ls1.minLevel = 10; ls1.isActive = true;
    s_loadingScreens[2001] = ls1;

    // Caves of Shadows
    LoadingScreen ls2;
    ls2.id = 2; ls2.instanceType = InstanceType::Dungeon; ls2.mapId = 2002;
    ls2.nameRu = "Пещеры Теней"; ls2.nameEn = "Caves of Shadows";
    ls2.backgroundImage = "bg_dungeon_shadows"; ls2.animationType = LoadingAnimationType::Slide;
    ls2.animationDurationMs = 3500; ls2.tipRu = "Остерегайтесь ловушек на полу.";
    ls2.tipEn = "Beware of floor traps."; ls2.musicTrack = "music_dungeon_dark";
    ls2.minLevel = 15; ls2.isActive = true;
    s_loadingScreens[2002] = ls2;

    // Ruins of the Ancients
    LoadingScreen ls3;
    ls3.id = 3; ls3.instanceType = InstanceType::Dungeon; ls3.mapId = 2003;
    ls3.nameRu = "Руины Древних"; ls3.nameEn = "Ruins of the Ancients";
    ls3.backgroundImage = "bg_dungeon_ruins"; ls3.animationType = LoadingAnimationType::Zoom;
    ls3.animationDurationMs = 4000; ls3.tipRu = "Ищите скрытые проходы за стенами.";
    ls3.tipEn = "Look for hidden passages behind walls."; ls3.musicTrack = "music_dungeon_epic";
    ls3.minLevel = 20; ls3.isActive = true;
    s_loadingScreens[2003] = ls3;

    // Raid: Citadel of Darkness
    LoadingScreen ls4;
    ls4.id = 4; ls4.instanceType = InstanceType::Raid; ls4.mapId = 3001;
    ls4.nameRu = "Рейд: Цитадель Тьмы"; ls4.nameEn = "Raid: Citadel of Darkness";
    ls4.backgroundImage = "bg_raid_citadel"; ls4.animationType = LoadingAnimationType::Particle;
    ls4.animationDurationMs = 5000; ls4.tipRu = "Координация группы критически важна.";
    ls4.tipEn = "Group coordination is critical."; ls4.musicTrack = "music_raid_epic";
    ls4.minLevel = 30; ls4.isActive = true;
    s_loadingScreens[3001] = ls4;

    // Raid: Dragon Lair
    LoadingScreen ls5;
    ls5.id = 5; ls5.instanceType = InstanceType::Raid; ls5.mapId = 3002;
    ls5.nameRu = "Рейд: Логово Дракона"; ls5.nameEn = "Raid: Dragon Lair";
    ls5.backgroundImage = "bg_raid_dragon"; ls5.animationType = LoadingAnimationType::Fade;
    ls5.animationDurationMs = 4500; ls5.tipRu = "Избегайте огненного дыхания дракона.";
    ls5.tipEn = "Avoid the dragon breath."; ls5.musicTrack = "music_raid_dragon";
    ls5.minLevel = 40; ls5.isActive = true;
    s_loadingScreens[3002] = ls5;

    // Instance: Rift
    LoadingScreen ls6;
    ls6.id = 6; ls6.instanceType = InstanceType::Instance; ls6.mapId = 4001;
    ls6.nameRu = "Инстанс: Разлом"; ls6.nameEn = "Instance: Rift";
    ls6.backgroundImage = "bg_instance_rift"; ls6.animationType = LoadingAnimationType::Slide;
    ls6.animationDurationMs = 3000; ls6.tipRu = "Разлом искажает пространство и время.";
    ls6.tipEn = "The rift distorts space and time."; ls6.musicTrack = "music_instance_mystic";
    ls6.minLevel = 25; ls6.isActive = true;
    s_loadingScreens[4001] = ls6;

    // Instance: Astral Plane
    LoadingScreen ls7;
    ls7.id = 7; ls7.instanceType = InstanceType::Instance; ls7.mapId = 4002;
    ls7.nameRu = "Инстанс: Астральный План"; ls7.nameEn = "Instance: Astral Plane";
    ls7.backgroundImage = "bg_instance_astral"; ls7.animationType = LoadingAnimationType::Zoom;
    ls7.animationDurationMs = 3500; ls7.tipRu = "Астральные существа невидимы для обычных атак.";
    ls7.tipEn = "Astral beings are invisible to normal attacks."; ls7.musicTrack = "music_instance_cosmic";
    ls7.minLevel = 35; ls7.isActive = true;
    s_loadingScreens[4002] = ls7;
}

void InstanceLoadingMgr::LoadConfig() {
    // Config already set in constructor
}

void InstanceLoadingMgr::LogLoading(uint32_t playerGuid, uint32_t mapId, uint32_t loadingTimeMs) {
    // TODO: Log to database
}

bool InstanceLoadingMgr::IsEnabled() const {
    return s_config.enabled;
}

uint32_t InstanceLoadingMgr::GetMinDisplayTime() const {
    return s_config.minDisplayTimeMs;
}

} // namespace dm