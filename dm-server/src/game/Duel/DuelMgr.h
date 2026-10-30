#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

namespace dm {

enum class DuelState : uint8_t {
    Idle = 0,
    Queue = 1,
    Duel = 2,
    Finished = 3
};

struct DuelParticipant {
    uint32_t playerGuid;
    uint8_t level;
    uint8_t spiritLevel;
    uint64_t spiritId;
    uint32_t ratingBefore;
    uint32_t ratingAfter;
    uint32_t damageDealt;
    uint32_t healingDone;
    uint8_t kills;
    uint8_t deaths;
    bool isAlive;
};

struct DuelMatch {
    uint64_t matchId;
    DuelState state;
    uint32_t startTime;
    uint32_t endTime;
    uint32_t duration; // in seconds
    DuelParticipant participant1;
    DuelParticipant participant2;
    uint8_t winner; // 1=player1, 2=player2, 0=draw
};

class DuelMgr {
public:
    static DuelMgr &Instance();

    // Очередь на дуэль
    bool JoinQueue(uint32_t playerGuid, uint8_t level, uint8_t spiritLevel, uint64_t spiritId);
    bool LeaveQueue(uint32_t playerGuid);
    bool IsInQueue(uint32_t playerGuid) const;
    std::vector<uint32_t> GetQueuePlayers() const;

    // Дуэль
    bool StartDuel(uint32_t player1Guid, uint32_t player2Guid);
    bool EndDuel(uint32_t matchId);
    DuelState GetState() const;
    bool IsDuelActive() const;

    // Участники
    bool AddParticipant(uint32_t matchId, uint32_t playerGuid, const DuelParticipant &participant);
    bool RemoveParticipant(uint32_t matchId, uint32_t playerGuid);
    DuelParticipant *GetParticipant(uint32_t matchId, uint32_t playerGuid);
    std::vector<DuelParticipant> GetParticipants(uint32_t matchId) const;

    // Статистика
    void RecordDamage(uint32_t matchId, uint32_t playerGuid, uint32_t amount);
    void RecordHealing(uint32_t matchId, uint32_t playerGuid, uint32_t amount);
    void RecordKill(uint32_t matchId, uint32_t killerGuid, uint32_t victimGuid);
    void SetPlacement(uint32_t matchId, uint32_t playerGuid, uint8_t placement);

    // Рейтинг
    void UpdateRating(uint32_t matchId, uint8_t result); // result: 1=win, 2=loss, 0=draw
    uint32_t GetRating(uint32_t playerGuid) const;
    uint32_t GetWins(uint32_t playerGuid) const;
    uint32_t GetLosses(uint32_t playerGuid) const;
    uint32_t GetDraws(uint32_t playerGuid) const;
    uint32_t GetDuelsPlayed(uint32_t playerGuid) const;

    // Конфигурация
    uint32_t GetMinDuelLevel() const { return minDuelLevel; }
    uint32_t GetMaxDuelLevelDiff() const { return maxDuelLevelDiff; }
    uint32_t GetDuelDuration() const { return duelDuration; }
    uint32_t GetCooldownMin() const { return cooldownMin; }
    uint32_t GetRewardSpiritExpWin() const { return rewardSpiritExpWin; }
    uint32_t GetRewardSpiritExpDraw() const { return rewardSpiritExpDraw; }
    uint8_t scheduleDay = 6;      // Day of week (0=Sun, 6=Sat)
    uint8_t scheduleHour = 18;    // Start hour
    uint8_t scheduleMinute = 0;   // Start minute

    // Проверки
    bool CanJoin(uint32_t playerGuid, uint8_t playerLevel, uint8_t spiritLevel) const;
    uint32_t GetDuelsToday(uint32_t playerGuid) const;
    void IncrementDuelsToday(uint32_t playerGuid);
    bool CanScheduleRun() const;

private:
    DuelMgr();
    ~DuelMgr();

    static DuelMgr *s_instance;

    // État de la file d'attente
    DuelState s_state = DuelState::Idle;
    std::unordered_set<uint32_t> s_queue;
    std::unordered_map<uint32_t, DuelParticipant> s_participants; // playerGuid -> participant

    // Configuration
    uint32_t minDuelLevel = 10;
    uint32_t maxDuelLevelDiff = 20;
    uint32_t duelDuration = 300; // 5 min in seconds
    uint32_t cooldownMin = 5;
    uint32_t rewardSpiritExpWin = 100;
    uint32_t rewardSpiritExpDraw = 50;

    // Journaux
    std::unordered_map<uint32_t, uint32_t> s_duelsToday; // playerGuid -> count today
};

} // namespace dm