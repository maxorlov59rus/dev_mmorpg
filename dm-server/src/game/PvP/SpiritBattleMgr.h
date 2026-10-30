// SpiritBattleMgr.h — PvP ивент "Битва Духов"
#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

namespace dm {

enum class SpiritBattleState : uint8_t {
    Idle = 0,
    Queue = 1,
    Battle = 2,
    Finished = 3
};

struct SpiritBattleParticipant {
    uint32_t playerGuid;
    uint64_t spiritId;
    uint32_t damageDealt;
    uint32_t healingDone;
    uint8_t kills;
    uint8_t deaths;
    uint8_t placement;
    bool isAlive;
};

struct SpiritBattle {
    uint64_t battleId;
    SpiritBattleState state;
    uint32_t startTime;
    uint32_t endTime;
    std::vector<SpiritBattleParticipant> participants;
    uint8_t winnerPlacement;
};

class SpiritBattleMgr {
public:
    static SpiritBattleMgr &Instance();

    // Очередь
    bool JoinQueue(uint32_t playerGuid, uint64_t spiritId);
    bool LeaveQueue(uint32_t playerGuid);
    bool IsInQueue(uint32_t playerGuid) const;
    std::vector<uint32_t> GetQueuePlayers() const;

    // Битва
    bool StartBattle();
    bool EndBattle();
    SpiritBattleState GetState() const;
    bool IsBattleActive() const;

    // Участники
    bool AddParticipant(uint32_t playerGuid, uint64_t spiritId);
    bool RemoveParticipant(uint32_t playerGuid);
    SpiritBattleParticipant *GetParticipant(uint32_t playerGuid);
    std::vector<SpiritBattleParticipant> GetParticipants() const;

    // Статистика
    void RecordDamage(uint32_t playerGuid, uint32_t amount);
    void RecordHealing(uint32_t playerGuid, uint32_t amount);
    void RecordKill(uint32_t killerGuid, uint32_t victimGuid);
    void SetPlacement(uint32_t playerGuid, uint8_t placement);

    // Награды
    void DistributeRewards();

    // Конфигурация
    uint32_t GetDailyLimit() const { return 3; }
    uint32_t GetMaxParticipants() const { return 100; }
    uint32_t GetBattleDuration() const { return 15 * 60; } // 15 min
    uint32_t GetQueueDuration() const { return 5 * 60; } // 5 min

    // Проверки
    bool CanJoin(uint32_t playerGuid, uint8_t playerLevel, uint8_t spiritLevel) const;
    uint32_t GetBattlesToday(uint32_t playerGuid) const;
    void IncrementBattlesToday(uint32_t playerGuid);
};

} // namespace dm