#pragma once

#include <string>

namespace dm {
namespace common {

class GameTime {
public:
    static GameTime& Instance();

    void Init(float speed, int startHour, int format, time_t offset = 0);
    std::string GetCurrentTime() const;
    int GetHour() const;
    int GetMinute() const;
    std::string GetFormattedTime() const;
    void Advance(float deltaSeconds);
    std::string ToString() const;

private:
    GameTime() = default;
    ~GameTime();

    float m_speed{1.0f};
    int m_hour{12};
    int m_minute{0};
    int m_format{12};
    time_t m_lastAdvance{0};
};

} // namespace common
} // namespace dm