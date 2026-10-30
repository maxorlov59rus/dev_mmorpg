#pragma once

namespace dm {
namespace common {

enum class WeatherType {
    CLEAR,
    RAIN,
    THUNDER,
    SNOW,
    FOG,
    SANDSTORM
};

enum class WeatherSeverity {
    LIGHT,
    MEDIUM,
    HEAVY
};

struct WeatherData {
    WeatherType type;
    WeatherSeverity severity;
    float duration;      // seconds until weather changes
    float elapsed;       // seconds weather has been active
    int radius;          // influence radius in units
    bool player_visible; // true if weather affects the player
};

// Generates weather based on map coordinates and game time
WeatherData GenerateWeather(float posX, float posZ, int64_t gameTime);

// Converts WeatherType to readable string
std::string WeatherTypeToString(WeatherType type);

// Converts WeatherSeverity to readable string
std::string WeatherSeverityToString(WeatherSeverity severity);

} // namespace common
} // namespace dm