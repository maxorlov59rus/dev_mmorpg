// RealTimeMgr.h — Синхронизация с реальным временем
#pragma once
#include <cstdint>
#include <string>
#include <ctime>

namespace dm {

struct RealTimeInfo {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int dayOfWeek; // 0=Sunday, 6=Saturday
    int dayOfYear;
    bool isWeekend;
    bool isDST;
};

class RealTimeMgr {
public:
    static RealTimeMgr &Instance();

    // Получение реального времени
    RealTimeInfo GetCurrentTime() const;
    uint32_t GetUnixTimestamp() const;
    int GetDayOfWeek() const;
    int GetHour() const;
    int GetMinute() const;
    bool IsWeekend() const;

    // Форматирование
    std::string GetFormattedTime() const;
    std::string GetFormattedDate() const;

    // Синхронизация с клиентом
    void SyncWithClient(uint32_t clientTimestamp);

    // Конфигурация
    int GetTimezoneOffset() const;
    void SetTimezoneOffset(int offsetMinutes);

    // Инициализация
    void Init();
};

} // namespace dm