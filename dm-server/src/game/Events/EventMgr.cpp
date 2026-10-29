#include "game/Events/EventMgr.h"
#include <unordered_map>

namespace dm {

EventMgr &EventMgr::Instance() {
    static EventMgr inst;
    return inst;
}

static std::unordered_map<uint32_t, WorldEvent> s_events;

void EventMgr::LoadEvent(const WorldEvent &ev) {
    s_events[ev.EventId] = ev;
}

WorldEvent *EventMgr::GetEvent(uint32_t eventId) {
    auto it = s_events.find(eventId);
    if (it == s_events.end())
        return nullptr;
    return &it->second;
}

void EventMgr::ScheduleEvent(uint32_t eventId, uint64_t startTime, uint64_t endTime) {
    WorldEvent *ev = GetEvent(eventId);
    if (ev) {
        ev->StartTime = startTime;
        ev->EndTime = endTime;
    }
}

void EventMgr::UpdateEvents(uint64_t currentTime) {
    for (auto &kv : s_events) {
        WorldEvent &ev = kv.second;
        if (!ev.Active && ev.StartTime > 0 && currentTime >= ev.StartTime &&
            currentTime <= ev.EndTime) {
            StartEvent(ev.EventId);
        }
        if (ev.Active && ev.EndTime > 0 && currentTime > ev.EndTime) {
            StopEvent(ev.EventId);
        }
    }
}

void EventMgr::StartEvent(uint32_t eventId) {
    WorldEvent *ev = GetEvent(eventId);
    if (ev)
        ev->Active = true;
}

void EventMgr::StopEvent(uint32_t eventId) {
    WorldEvent *ev = GetEvent(eventId);
    if (ev)
        ev->Active = false;
}

std::vector<WorldEvent> EventMgr::GetActiveEvents() const {
    std::vector<WorldEvent> res;
    for (const auto &kv : s_events) {
        if (kv.second.Active)
            res.push_back(kv.second);
    }
    return res;
}

} // namespace dm
