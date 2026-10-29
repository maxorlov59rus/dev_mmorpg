#include "game/PvPEvents/PvPEventMgr.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace dm {

PvPEventMgr &PvPEventMgr::Instance() {
    static PvPEventMgr inst;
    return inst;
}

static std::unordered_map<uint32_t, PvPEvent> s_events;
static std::unordered_map<uint32_t, std::vector<PvPParticipant>> s_participants;

void PvPEventMgr::LoadEvent(const PvPEvent &ev) {
    s_events[ev.EventId] = ev;
}

PvPEvent *PvPEventMgr::GetEvent(uint32_t eventId) {
    auto it = s_events.find(eventId);
    if (it == s_events.end()) return nullptr;
    return &it->second;
}

std::vector<PvPEvent> PvPEventMgr::GetAvailableEvents(uint8_t playerLevel) const {
    std::vector<PvPEvent> res;
    for (const auto &kv : s_events) {
        if (playerLevel >= kv.second.MinLevel && playerLevel <= kv.second.MaxLevel)
            res.push_back(kv.second);
    }
    return res;
}

uint32_t PvPEventMgr::JoinEvent(uint32_t eventId, uint64_t playerGuid, uint8_t level, uint32_t rating) {
    PvPParticipant p;
    p.Guid = playerGuid;
    p.Level = level;
    p.Rating = rating;
    s_participants[eventId].push_back(p);
    return eventId;
}

void PvPEventMgr::LeaveEvent(uint32_t eventId, uint64_t playerGuid) {
    auto it = s_participants.find(eventId);
    if (it == s_participants.end()) return;
    auto &vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [playerGuid](const PvPParticipant &p) { return p.Guid == playerGuid; }), vec.end());
}

std::vector<uint64_t> PvPEventMgr::GetParticipants(uint32_t eventId) const {
    std::vector<uint64_t> res;
    auto it = s_participants.find(eventId);
    if (it == s_participants.end()) return res;
    for (const auto &p : it->second) res.push_back(p.Guid);
    return res;
}

std::vector<uint64_t> PvPEventMgr::Matchmaking(uint32_t eventId, uint8_t playerLevel, uint32_t rating) {
    std::vector<uint64_t> result;
    auto it = s_participants.find(eventId);
    if (it == s_participants.end()) return result;
    for (const auto &p : it->second) {
        if (std::abs((int)p.Level - (int)playerLevel) <= 2 &&
            std::abs((int)p.Rating - (int)rating) <= 200) {
            result.push_back(p.Guid);
        }
    }
    return result;
}

} // namespace dm
