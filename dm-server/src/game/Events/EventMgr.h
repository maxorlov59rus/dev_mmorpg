// EventMgr.h — мировые эвенты
#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace dm {

struct WorldEvent {
    uint32_t EventId{0};
    std::string Name;
    bool Active{false};
    uint64_t StartTime{0};
    uint64_t EndTime{0};
};

class EventMgr {
public:
    static EventMgr &Instance();
    void LoadEvent(const WorldEvent &ev);
    WorldEvent *GetEvent(uint32_t eventId);
    void ScheduleEvent(uint32_t eventId, uint64_t startTime, uint64_t endTime);
    void UpdateEvents(uint64_t currentTime);
    void StartEvent(uint32_t eventId);
    void StopEvent(uint32_t eventId);
    std::vector<WorldEvent> GetActiveEvents() const;
};

} // namespace dm
