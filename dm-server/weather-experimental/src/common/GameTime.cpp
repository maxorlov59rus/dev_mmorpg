#include "GameTime.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace dm {
namespace common {

GameTime& GameTime::Instance() {
    static GameTime inst;
    return inst;
}

GameTime::~GameTime() {}

void GameTime::Init(float speed, int startHour, int format, time_t offset) {
    m_speed = speed;
    m_format = format;
    m_lastAdvance = std::time(nullptr) + offset;
    m_hour = startHour % 24;
    m_minute = 0;
}

std::string GameTime::GetCurrentTime() const {
    if (m_format == 24) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << m_hour << ":"
            << std::setfill('0') << std::setw(2) << m_minute;
        return oss.str();
    } else {
        int h12 = m_hour;
        std::string ampm = "AM";
        if (h12 == 0) h12 = 12;
        if (h12 >= 12) { ampm = "PM"; h12 -= 12; }
        if (h12 == 0) h12 = 12;
        std::ostringstream oss;
        oss << h12 << ":" << std::setfill('0') << std::setw(2) << m_minute << " " << ampm;
        return oss.str();
    }
}

int GameTime::GetHour() const { return m_hour; }
int GameTime::GetMinute() const { return m_minute; }

std::string GameTime::GetFormattedTime() const {
    return GetCurrentTime();
}

void GameTime::Advance(float deltaSeconds) {
    float realDelta = deltaSeconds * m_speed;
    int64_t deltaInt = (int64_t)realDelta;
    m_minute += deltaInt;
    while (m_minute >= 60) {
        m_minute -= 60;
        m_hour++;
        while (m_hour >= 24) m_hour -= 24;
    }
    m_lastAdvance += deltaInt;
}

std::string GameTime::ToString() const {
    return GetFormattedTime();
}