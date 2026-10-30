// InstanceLoadingMgr.h — Динамические анимированные логинскрины
#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

namespace dm {

enum class LoadingAnimationType : uint8_t {
    Fade = 1,
    Slide = 2,
    Zoom = 3,
    Particle = 4
};

enum class InstanceType : uint8_t {
    Dungeon = 1,
    Raid = 2,
    Instance = 3
};

struct LoadingScreen {
    uint32_t id;
    InstanceType instanceType;
    uint32_t mapId;
    std::string nameRu;
    std::string nameEn;
    std::string backgroundImage;
    LoadingAnimationType animationType;
    uint32_t animationDurationMs;
    std::string tipRu;
    std::string tipEn;
    std::string musicTrack;
    uint8_t minLevel;
    bool isActive;
};

struct LoadingScreenConfig {
    bool enabled;
    LoadingAnimationType defaultAnimation;
    uint32_t defaultDurationMs;
    bool showTips;
    bool showProgressBar;
    uint32_t fadeInMs;
    uint32_t fadeOutMs;
    uint32_t minDisplayTimeMs;
};

class InstanceLoadingMgr {
public:
    static InstanceLoadingMgr &Instance();

    // Логинскрины
    const LoadingScreen *GetLoadingScreen(uint32_t mapId) const;
    std::vector<LoadingScreen> GetAllLoadingScreens() const;
    bool HasLoadingScreen(uint32_t mapId) const;

    // Конфигурация
    LoadingScreenConfig GetConfig() const;
    void SetConfig(const LoadingScreenConfig &config);

    // Загрузка данных
    void LoadLoadingScreens();
    void LoadConfig();

    // Логирование
    void LogLoading(uint32_t playerGuid, uint32_t mapId, uint32_t loadingTimeMs);

    // Проверки
    bool IsEnabled() const;
    uint32_t GetMinDisplayTime() const;
};

} // namespace dm