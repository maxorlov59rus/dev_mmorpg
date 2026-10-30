// RelationshipMgr.h — Система отношений: друзья, пара, брак
#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

namespace dm {

enum class RelationshipType : uint8_t {
    Acquaintance = 0,
    Couple = 1,
    Engaged = 2,
    Married = 3
};

enum class RelationshipStatus : uint8_t {
    Pending = 0,
    Active = 1,
    Dissolved = 2
};

enum class FriendshipStatus : uint8_t {
    Pending = 0,
    Accepted = 1,
    Blocked = 2
};

struct FriendEntry {
    uint32_t friendGuid;
    FriendshipStatus status;
    uint32_t createdAt;
    uint32_t acceptedAt;
};

struct RelationshipEntry {
    uint32_t player1Guid;
    uint32_t player2Guid;
    RelationshipType type;
    RelationshipStatus status;
    uint32_t createdAt;
    uint32_t acceptedAt;
    uint32_t dissolvedAt;
};

class RelationshipMgr {
public:
    static RelationshipMgr &Instance();

    // Друзья
    bool SendFriendRequest(uint32_t playerGuid, uint32_t targetGuid);
    bool AcceptFriendRequest(uint32_t playerGuid, uint32_t requesterGuid);
    bool DeclineFriendRequest(uint32_t playerGuid, uint32_t requesterGuid);
    bool RemoveFriend(uint32_t playerGuid, uint32_t friendGuid);
    bool BlockPlayer(uint32_t playerGuid, uint32_t targetGuid);
    std::vector<FriendEntry> GetFriends(uint32_t playerGuid) const;
    bool IsFriend(uint32_t playerGuid, uint32_t targetGuid) const;
    uint32_t GetFriendCount(uint32_t playerGuid) const;

    // Отношения
    bool SendRelationshipRequest(uint32_t playerGuid, uint32_t targetGuid, RelationshipType type);
    bool AcceptRelationship(uint32_t playerGuid, uint32_t requesterGuid);
    bool DeclineRelationship(uint32_t playerGuid, uint32_t requesterGuid);
    bool DissolveRelationship(uint32_t playerGuid, uint32_t partnerGuid);
    RelationshipEntry *GetActiveRelationship(uint32_t playerGuid);
    bool HasActiveRelationship(uint32_t playerGuid) const;
    std::vector<RelationshipEntry> GetRelationshipHistory(uint32_t playerGuid) const;

    // Пол персонажа
    uint8_t GetGender(uint32_t playerGuid) const;
    void SetGender(uint32_t playerGuid, uint8_t gender);

    // Проверки
    bool CanFormRelationship(uint32_t playerGuid, uint32_t targetGuid, RelationshipType type) const;
    bool IsCouple(uint32_t playerGuid, uint32_t targetGuid) const;
    bool IsMarried(uint32_t playerGuid, uint32_t targetGuid) const;
};

} // namespace dm