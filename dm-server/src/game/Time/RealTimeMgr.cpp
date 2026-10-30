#include "game/Time/RealTimeMgr.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace dm {

static int s_timezoneOffset = 0; // minutes

RealTimeMgr &RealTimeMgr::Instance() {
    static RealTimeMgr inst;
    return inst;
}

void RealTimeMgr::Init() {
    // Nothing special needed, uses system time
}

RealTimeInfo RealTimeMgr::GetCurrentTime() const {
    time_t now = time(nullptr);
    now += s_timezoneOffset * 60;
    struct tm *t = gmtime(&now);

    RealTimeInfo info;
    info.year = t->tm_year + 1900;
    info.month = t->tm_mon + 1;
    info.day = t->tm_mday;
    info.hour = t->tm_hour;
    info.minute = t->tm_min;
    info.second = t->tm_sec;
    info.dayOfWeek = t->tm_wday;
    info.dayOfYear = t->tm_yday;
    info.isWeekend = (t->tm_wday == 0 || t->tm_wday == 6);
    info.isDST = (t->tm_isdst > 0);
    return info;
}

uint32_t RealTimeMgr::GetUnixTimestamp() const {
    return static_cast<uint32_t>(time(nullptr));
}

int RealTimeMgr::GetDayOfWeek() const {
    return GetCurrentTime().dayOfWeek;
}

int RealTimeMgr::GetHour() const {
    return GetCurrentTime().hour;
}

int RealTimeMgr::GetMinute() const {
    return GetCurrentTime().minute;
}

bool RealTimeMgr::IsWeekend() const {
    return GetCurrentTime().isWeekend;
}

std::string RealTimeMgr::GetFormattedTime() const {
    RealTimeInfo info = GetCurrentTime();
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << info.hour << ":"
        << std::setfill('0') << std::setw(2) << info.minute << ":"
        << std::setfill('0') << std::setw(2) << info.second;
    return oss.str();
}

std::string RealTimeMgr::GetFormattedDate() const {
    RealTimeInfo info = GetCurrentTime();
    std::ostringstream oss;
    oss << info.year << "-"
        << std::setfill('0') << std::setw(2) << info.month << "-"
        << std::setfill('0') << std::setw(2) << info.day;
    return oss.str();
}

void RealTimeMgr::SyncWithClient(uint32_t clientTimestamp) {
    // TODO: Compare client timestamp with server time
    // If difference is too large, send correction
}

int RealTimeMgr::GetTimezoneOffset() const {
    return s_timezoneOffset;
}

void RealTimeMgr::SetTimezoneOffset(int offsetMinutes) {
    s_timezoneOffset = offsetMinutes;
}

} // namespace dm