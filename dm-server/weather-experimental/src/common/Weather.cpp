#include "Weather.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

namespace dm {
namespace common {

// Simple deterministic hash from coords + time
static int FastHash(float x, float z, int64_t time) {
    int h = (int)(x * 127.9898f + z * 78.233f + time);
    return (h & 0x7fffffff);
}

// Generates weather based on position and game time
WeatherData GenerateWeather(float posX, float posZ, int64_t gameTime) {
    WeatherData w;

    // Use seed for reproducible "randomness"
    int seed = FastHash(posX, posZ, gameTime);
    srand(seed);

    int r = rand() % 100;

    if (r < 20) {
        w.type = WeatherType::CLEAR;
        w.severity = WeatherSeverity::LIGHT;
        w.duration = 300 + (rand() % 600); // 5-11 min
    } else if (r < 50) {
        w.type = WeatherType::RAIN;
        w.severity = WeatherSeverity::Medium;
        w.duration = 200 + (rand() % 400); // 3-7 min
    } else if (r < 65) {
        w.type = WeatherType::THUNDER;
        w.severity = WeatherSeverity::Heavy;
        w.duration = 100 + (rand() % 200); // 1-4 min
    } else if (r < 90) {
        w.type = WeatherType::SNOW;
        w.severity = WeatherSeverity::Light;
        w.duration = 400 + (rand() % 800); // 6-14 min
    } else {
        w.type = WeatherType::FOG;
        w.severity = WeatherSeverity::Medium;
        w.duration = 250 + (rand() % 500); // 4-9 min
    }

    w.elapsed = 0;
    w.radius = 500;
    w.player_visible = true;
    return w;
}

// Converts WeatherType to string
std::string WeatherTypeToString(WeatherType type) {
    switch (type) {
        case WeatherType::CLEAR: return "CLEAR";
        case WeatherType::RAIN: return "RAIN";
        case WeatherType::THUNDER: return "THUNDER";
        case WeatherType::SNOW: return "SNOW";
        case WeatherType::FOG: return "FOG";
        case WeatherType::SANDSTORM: return "SANDSTORM";
        default: return "UNKNOWN";
    }
}

// Converts WeatherSeverity to string
std::string WeatherSeverityToString(WeatherSeverity severity) {
    switch (severity) {
        case WeatherSeverity::LIGHT: return "LIGHT";
        case WeatherSeverity::MEDIUM: return "MEDIUM";
        case WeatherSeverity::HEAVY: return "HEAVY";
        default: return "UNKNOWN";
    }
}