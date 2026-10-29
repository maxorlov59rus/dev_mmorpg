// PvPEventMgr.h — PvP-события (бг, арены)
#pragma once
#include <cstdint>
#include <vector>

namespace dm {

struct PvPEvent {
    uint32_t EventId{0};
    uint8_t Type{0};
    uint8_t MinLevel{1};
    uint8_t MaxLevel{95};
    uint32_t RewardHonor{0};
    uint32_t RewardItem{0};
};

struct PvPParticipant {
    uint64_t Guid{0};
    uint8_t Level{1};
    uint32_t Rating{0};
};

class PvPEventMgr {
public:
    static PvPEventMgr &Instance();
    void LoadEvent(const PvPEvent &ev);
    PvPEvent *GetEvent(uint32_t eventId);
    std::vector<PvPEvent> GetAvailableEvents(uint8_t playerLevel) const;
    uint32_t JoinEvent(uint32_t eventId, uint64_t playerGuid, uint8_t level, uint32_t rating);
    void LeaveEvent(uint32_t eventId, uint64_t playerGuid);
    std::vector<uint64_t> GetParticipants(uint32_t eventId) const;
    std::vector<uint64_t> Matchmaking(uint32_t eventId, uint8_t playerLevel, uint32_t rating);
};

} // namespace dm
