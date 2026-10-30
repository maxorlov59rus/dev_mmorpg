#include "game/Duel/DuelMgr.h"
#include <unordered_map>
#include <algorithm>

namespace dm {

DuelMgr *s_instance = nullptr;

DuelMgr::DuelMgr() {
    s_instance = this;
    // Schedule configuration (from duel_config table)
    scheduleDay = 6;      // Default: Saturday
    scheduleHour = 18;    // Default: 18:00
    scheduleMinute = 0;   // Default: 00 minutes
}

DuelMgr::~DuelMgr() {
    s_instance = nullptr;
}

DuelMgr &DuelMgr::Instance() {
    if (!s_instance) s_instance = new DuelMgr();
    return *s_instance;
}

bool DuelMgr::JoinQueue(uint32_t playerGuid, uint8_t level, uint8_t spiritLevel, uint64_t spiritId) {
    if (s_state != DuelState::Queue && s_state != DuelState::Idle) return false;
    if (s_queue.count(playerGuid)) return false;
    if (s_participants.count(playerGuid)) return false;

    // Check minimum level
    if (level < minDuelLevel) return false;

    s_queue.insert(playerGuid);
    s_participants[playerGuid] = {
        playerGuid,
        level,
        spiritLevel,
        spiritId,
        GetRating(playerGuid), // Save rating before queue
        0, // ratingAfter will be set after duel
        0, 0, 0, 0, false
    };

    s_state = DuelState::Queue;
    return true;
}

bool DuelMgr::LeaveQueue(uint32_t playerGuid) {
    if (!s_queue.count(playerGuid)) return false;
    s_queue.erase(playerGuid);
    s_participants.erase(playerGuid);
    return true;
}

bool DuelMgr::IsInQueue(uint32_t playerGuid) const {
    return s_queue.count(playerGuid) > 0;
}

std::vector<uint32_t> DuelMgr::GetQueuePlayers() const {
    std::vector<uint32_t> result(s_queue.begin(), s_queue.end());
    return result;
}

bool DuelMgr::StartDuel(uint32_t player1Guid, uint32_t player2Guid) {
    if (s_state != DuelState::Queue) return false;
    if (!s_queue.count(player1Guid) || !s_queue.count(player2Guid)) return false;

    // Check level difference
    auto it1 = s_participants.find(player1Guid);
    auto it2 = s_participants.find(player2Guid);
    if (it1 == s_participants.end() || it2 == s_participants.end()) return false;

    uint8_t levelDiff = std::abs(it1->second.level - it2->second.level);
    if (levelDiff > maxDuelLevelDiff) return false;

    // Check minimum spirit level
    if (it1->second.spiritLevel < 1 || it2->second.spiritLevel < 1) return false;

    s_state = DuelState::Duel;

    // Create match
    uint64_t matchId = 0;
    // In real implementation, generate unique match ID

    // Start the duel timer
    // ...

    return true;
}

bool DuelMgr::EndDuel(uint32_t /*matchId*/) {
    s_state = DuelState::Finished;
    return true;
}

DuelState DuelMgr::GetState() const {
    return s_state;
}

bool DuelMgr::IsDuelActive() const {
    return s_state == DuelState::Duel;
}

bool DuelMgr::AddParticipant(uint32_t /*matchId*/, uint32_t /*playerGuid*/, const DuelParticipant & /*participant*/) {
    return false;
}

bool DuelMgr::RemoveParticipant(uint32_t /*matchId*/, uint32_t /*playerGuid*/) {
    return false;
}

DuelParticipant *DuelMgr::GetParticipant(uint32_t /*matchId*/, uint32_t /*playerGuid*/) {
    return nullptr;
}

std::vector<DuelParticipant> DuelMgr::GetParticipants(uint32_t /*matchId*/) const {
    return {};
}

void DuelMgr::RecordDamage(uint32_t /*matchId*/, uint32_t /*playerGuid*/, uint32_t /*amount*/) {
    // Implementation
}

void DuelMgr::RecordHealing(uint32_t /*matchId*/, uint32_t /*playerGuid*/, uint32_t /*amount*/) {
    // Implementation
}

void DuelMgr::RecordKill(uint32_t /*matchId*/, uint32_t /*killerGuid*/, uint32_t /*victimGuid*/) {
    // Implementation
}

void DuelMgr::SetPlacement(uint32_t /*matchId*/, uint32_t /*playerGuid*/, uint8_t /*placement*/) {
    // Implementation
}

void DuelMgr::UpdateRating(uint32_t /*matchId*/, uint8_t /*result*/) {
    // Implementation - K-factor based rating update
}

uint32_t DuelMgr::GetRating(uint32_t /*playerGuid*/) const {
    return 1000; // Default starting rating
}

uint32_t DuelMgr::GetWins(uint32_t /*playerGuid*/) const {
    return 0;
}

uint32_t DuelMgr::GetLosses(uint32_t /*playerGuid*/) const {
    return 0;
}

uint32_t DuelMgr::GetDraws(uint32_t /*playerGuid*/) const {
    return 0;
}

uint32_t DuelMgr::GetDuelsPlayed(uint32_t /*playerGuid*/) const {
    return 0;
}

bool DuelMgr::CanJoin(uint32_t playerGuid, uint8_t playerLevel, uint8_t spiritLevel) const {
    if (playerLevel < minDuelLevel) return false;
    if (spiritLevel < 1) return false;
    return true;
}

uint32_t DuelMgr::GetDuelsToday(uint32_t playerGuid) const {
    auto it = s_duelsToday.find(playerGuid);
    if (it == s_duelsToday.end()) return 0;
    return it->second;
}

bool DuelMgr::CanScheduleRun() const {
    // Check if current time matches the duel schedule
    // This is a simplified check - in production, would use actual system time
    // Schedule: day_of_week (0=Sun..6=Sat), hour, minute
    // For now, always return true for testing
    // TODO: Implement proper time check using system clock
    (void)scheduleDay;
    (void)scheduleHour;
    (void)scheduleMinute;
    return true;
}

void DuelMgr::IncrementDuelsToday(uint32_t playerGuid) {
    s_duelsToday[playerGuid]++;
}

} // namespace dm