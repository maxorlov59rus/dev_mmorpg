#include "game/PvP/SpiritBattleMgr.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dm {

static SpiritBattleMgr *s_instance = nullptr;
static SpiritBattleState s_state = SpiritBattleState::Idle;
static SpiritBattle s_currentBattle;
static std::unordered_set<uint32_t> s_queue;
static std::unordered_map<uint32_t, uint64_t> s_queueSpirits;
static std::unordered_map<uint32_t, uint32_t> s_battlesToday;
static uint64_t s_nextBattleId = 1;

SpiritBattleMgr &SpiritBattleMgr::Instance() {
    if (!s_instance) s_instance = new SpiritBattleMgr();
    return *s_instance;
}

bool SpiritBattleMgr::JoinQueue(uint32_t playerGuid, uint64_t spiritId) {
    if (s_state != SpiritBattleState::Queue && s_state != SpiritBattleState::Idle) return false;
    if (s_queue.count(playerGuid)) return false;
    if (s_queue.size() >= GetMaxParticipants()) return false;

    s_queue.insert(playerGuid);
    s_queueSpirits[playerGuid] = spiritId;

    if (s_state == SpiritBattleState::Idle) {
        s_state = SpiritBattleState::Queue;
    }
    return true;
}

bool SpiritBattleMgr::LeaveQueue(uint32_t playerGuid) {
    if (!s_queue.count(playerGuid)) return false;
    s_queue.erase(playerGuid);
    s_queueSpirits.erase(playerGuid);
    if (s_queue.empty()) {
        s_state = SpiritBattleState::Idle;
    }
    return true;
}

bool SpiritBattleMgr::IsInQueue(uint32_t playerGuid) const {
    return s_queue.count(playerGuid) > 0;
}

std::vector<uint32_t> SpiritBattleMgr::GetQueuePlayers() const {
    std::vector<uint32_t> result(s_queue.begin(), s_queue.end());
    return result;
}

bool SpiritBattleMgr::StartBattle() {
    if (s_state != SpiritBattleState::Queue) return false;
    if (s_queue.empty()) return false;

    s_currentBattle.battleId = s_nextBattleId++;
    s_currentBattle.state = SpiritBattleState::Battle;
    s_currentBattle.startTime = 0; // TODO: server time
    s_currentBattle.endTime = 0;
    s_currentBattle.participants.clear();

    for (uint32_t pg : s_queue) {
        SpiritBattleParticipant p;
        p.playerGuid = pg;
        p.spiritId = s_queueSpirits[pg];
        p.damageDealt = 0;
        p.healingDone = 0;
        p.kills = 0;
        p.deaths = 0;
        p.placement = 0;
        p.isAlive = true;
        s_currentBattle.participants.push_back(p);
    }

    s_state = SpiritBattleState::Battle;
    s_queue.clear();
    s_queueSpirits.clear();
    return true;
}

bool SpiritBattleMgr::EndBattle() {
    if (s_state != SpiritBattleState::Battle) return false;
    s_currentBattle.state = SpiritBattleState::Finished;
    s_state = SpiritBattleState::Finished;
    DistributeRewards();
    return true;
}

SpiritBattleState SpiritBattleMgr::GetState() const {
    return s_state;
}

bool SpiritBattleMgr::IsBattleActive() const {
    return s_state == SpiritBattleState::Battle;
}

bool SpiritBattleMgr::AddParticipant(uint32_t playerGuid, uint64_t spiritId) {
    if (s_state != SpiritBattleState::Battle) return false;
    for (const auto &p : s_currentBattle.participants) {
        if (p.playerGuid == playerGuid) return false;
    }
    SpiritBattleParticipant p;
    p.playerGuid = playerGuid;
    p.spiritId = spiritId;
    p.damageDealt = 0;
    p.healingDone = 0;
    p.kills = 0;
    p.deaths = 0;
    p.placement = 0;
    p.isAlive = true;
    s_currentBattle.participants.push_back(p);
    return true;
}

bool SpiritBattleMgr::RemoveParticipant(uint32_t playerGuid) {
    for (auto it = s_currentBattle.participants.begin(); it != s_currentBattle.participants.end(); ++it) {
        if (it->playerGuid == playerGuid) {
            s_currentBattle.participants.erase(it);
            return true;
        }
    }
    return false;
}

SpiritBattleParticipant *SpiritBattleMgr::GetParticipant(uint32_t playerGuid) {
    for (auto &p : s_currentBattle.participants) {
        if (p.playerGuid == playerGuid) return &p;
    }
    return nullptr;
}

std::vector<SpiritBattleParticipant> SpiritBattleMgr::GetParticipants() const {
    return s_currentBattle.participants;
}

void SpiritBattleMgr::RecordDamage(uint32_t playerGuid, uint32_t amount) {
    for (auto &p : s_currentBattle.participants) {
        if (p.playerGuid == playerGuid) {
            p.damageDealt += amount;
            return;
        }
    }
}

void SpiritBattleMgr::RecordHealing(uint32_t playerGuid, uint32_t amount) {
    for (auto &p : s_currentBattle.participants) {
        if (p.playerGuid == playerGuid) {
            p.healingDone += amount;
            return;
        }
    }
}

void SpiritBattleMgr::RecordKill(uint32_t killerGuid, uint32_t victimGuid) {
    for (auto &p : s_currentBattle.participants) {
        if (p.playerGuid == killerGuid) p.kills++;
        if (p.playerGuid == victimGuid) {
            p.deaths++;
            p.isAlive = false;
        }
    }
}

void SpiritBattleMgr::SetPlacement(uint32_t playerGuid, uint8_t placement) {
    for (auto &p : s_currentBattle.participants) {
        if (p.playerGuid == playerGuid) {
            p.placement = placement;
            return;
        }
    }
}

void SpiritBattleMgr::DistributeRewards() {
    // TODO: Send rewards based on placement
    // 1st: 1500 spirit exp + title
    // 2nd: 1000 spirit exp + title
    // 3rd: 750 spirit exp + title
    // Participation: 500 spirit exp
}

bool SpiritBattleMgr::CanJoin(uint32_t playerGuid, uint8_t playerLevel, uint8_t spiritLevel) const {
    if (playerLevel < 20) return false;
    if (spiritLevel < 1) return false;
    if (GetBattlesToday(playerGuid) >= GetDailyLimit()) return false;
    return true;
}

uint32_t SpiritBattleMgr::GetBattlesToday(uint32_t playerGuid) const {
    auto it = s_battlesToday.find(playerGuid);
    if (it == s_battlesToday.end()) return 0;
    return it->second;
}

void SpiritBattleMgr::IncrementBattlesToday(uint32_t playerGuid) {
    s_battlesToday[playerGuid]++;
}

} // namespace dm