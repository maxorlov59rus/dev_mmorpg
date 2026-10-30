#include "game/Social/RelationshipMgr.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dm {

static std::unordered_map<uint32_t, std::vector<FriendEntry>> s_friends;
static std::unordered_map<uint32_t, std::vector<RelationshipEntry>> s_relationships;
static std::unordered_map<uint32_t, uint8_t> s_gender;

RelationshipMgr &RelationshipMgr::Instance() {
    static RelationshipMgr inst;
    return inst;
}

bool RelationshipMgr::SendFriendRequest(uint32_t playerGuid, uint32_t targetGuid) {
    if (playerGuid == targetGuid) return false;
    if (IsFriend(playerGuid, targetGuid)) return false;
    if (GetFriendCount(playerGuid) >= 50) return false;

    FriendEntry entry;
    entry.friendGuid = targetGuid;
    entry.status = FriendshipStatus::Pending;
    entry.createdAt = 0;
    entry.acceptedAt = 0;
    s_friends[playerGuid].push_back(entry);
    return true;
}

bool RelationshipMgr::AcceptFriendRequest(uint32_t playerGuid, uint32_t requesterGuid) {
    auto &friends = s_friends[playerGuid];
    for (auto &f : friends) {
        if (f.friendGuid == requesterGuid && f.status == FriendshipStatus::Pending) {
            f.status = FriendshipStatus::Accepted;
            f.acceptedAt = 0;

            FriendEntry reverse;
            reverse.friendGuid = playerGuid;
            reverse.status = FriendshipStatus::Accepted;
            reverse.createdAt = f.createdAt;
            reverse.acceptedAt = f.acceptedAt;
            s_friends[requesterGuid].push_back(reverse);
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::DeclineFriendRequest(uint32_t playerGuid, uint32_t requesterGuid) {
    auto &friends = s_friends[playerGuid];
    for (auto it = friends.begin(); it != friends.end(); ++it) {
        if (it->friendGuid == requesterGuid && it->status == FriendshipStatus::Pending) {
            friends.erase(it);
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::RemoveFriend(uint32_t playerGuid, uint32_t friendGuid) {
    auto &friends = s_friends[playerGuid];
    for (auto it = friends.begin(); it != friends.end(); ++it) {
        if (it->friendGuid == friendGuid) {
            friends.erase(it);
            auto &revFriends = s_friends[friendGuid];
            for (auto rit = revFriends.begin(); rit != revFriends.end(); ++rit) {
                if (rit->friendGuid == playerGuid) {
                    revFriends.erase(rit);
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::BlockPlayer(uint32_t playerGuid, uint32_t targetGuid) {
    RemoveFriend(playerGuid, targetGuid);
    FriendEntry entry;
    entry.friendGuid = targetGuid;
    entry.status = FriendshipStatus::Blocked;
    entry.createdAt = 0;
    entry.acceptedAt = 0;
    s_friends[playerGuid].push_back(entry);
    return true;
}

std::vector<FriendEntry> RelationshipMgr::GetFriends(uint32_t playerGuid) const {
    auto it = s_friends.find(playerGuid);
    if (it == s_friends.end()) return {};
    return it->second;
}

bool RelationshipMgr::IsFriend(uint32_t playerGuid, uint32_t targetGuid) const {
    auto it = s_friends.find(playerGuid);
    if (it == s_friends.end()) return false;
    for (const auto &f : it->second) {
        if (f.friendGuid == targetGuid && f.status == FriendshipStatus::Accepted) return true;
    }
    return false;
}

uint32_t RelationshipMgr::GetFriendCount(uint32_t playerGuid) const {
    auto it = s_friends.find(playerGuid);
    if (it == s_friends.end()) return 0;
    uint32_t count = 0;
    for (const auto &f : it->second) {
        if (f.status == FriendshipStatus::Accepted) count++;
    }
    return count;
}

bool RelationshipMgr::SendRelationshipRequest(uint32_t playerGuid, uint32_t targetGuid, RelationshipType type) {
    if (playerGuid == targetGuid) return false;
    if (HasActiveRelationship(playerGuid)) return false;
    if (HasActiveRelationship(targetGuid)) return false;

    RelationshipEntry entry;
    entry.player1Guid = playerGuid;
    entry.player2Guid = targetGuid;
    entry.type = type;
    entry.status = RelationshipStatus::Pending;
    entry.createdAt = 0;
    entry.acceptedAt = 0;
    entry.dissolvedAt = 0;
    s_relationships[playerGuid].push_back(entry);
    return true;
}

bool RelationshipMgr::AcceptRelationship(uint32_t playerGuid, uint32_t requesterGuid) {
    auto &rels = s_relationships[playerGuid];
    for (auto &r : rels) {
        if (r.player1Guid == requesterGuid && r.status == RelationshipStatus::Pending) {
            r.status = RelationshipStatus::Active;
            r.acceptedAt = 0;

            RelationshipEntry reverse;
            reverse.player1Guid = playerGuid;
            reverse.player2Guid = requesterGuid;
            reverse.type = r.type;
            reverse.status = RelationshipStatus::Active;
            reverse.createdAt = r.createdAt;
            reverse.acceptedAt = r.acceptedAt;
            reverse.dissolvedAt = 0;
            s_relationships[requesterGuid].push_back(reverse);
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::DeclineRelationship(uint32_t playerGuid, uint32_t requesterGuid) {
    auto &rels = s_relationships[playerGuid];
    for (auto it = rels.begin(); it != rels.end(); ++it) {
        if (it->player1Guid == requesterGuid && it->status == RelationshipStatus::Pending) {
            rels.erase(it);
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::DissolveRelationship(uint32_t playerGuid, uint32_t partnerGuid) {
    auto &rels = s_relationships[playerGuid];
    for (auto &r : rels) {
        if (r.player2Guid == partnerGuid && r.status == RelationshipStatus::Active) {
            r.status = RelationshipStatus::Dissolved;
            r.dissolvedAt = 0;

            auto &revRels = s_relationships[partnerGuid];
            for (auto &rr : revRels) {
                if (rr.player2Guid == playerGuid && rr.status == RelationshipStatus::Active) {
                    rr.status = RelationshipStatus::Dissolved;
                    rr.dissolvedAt = r.dissolvedAt;
                }
            }
            return true;
        }
    }
    return false;
}

RelationshipEntry *RelationshipMgr::GetActiveRelationship(uint32_t playerGuid) {
    auto it = s_relationships.find(playerGuid);
    if (it == s_relationships.end()) return nullptr;
    for (auto &r : it->second) {
        if (r.status == RelationshipStatus::Active) return &r;
    }
    return nullptr;
}

bool RelationshipMgr::HasActiveRelationship(uint32_t playerGuid) const {
    auto it = s_relationships.find(playerGuid);
    if (it == s_relationships.end()) return false;
    for (const auto &r : it->second) {
        if (r.status == RelationshipStatus::Active) return true;
    }
    return false;
}

std::vector<RelationshipEntry> RelationshipMgr::GetRelationshipHistory(uint32_t playerGuid) const {
    auto it = s_relationships.find(playerGuid);
    if (it == s_relationships.end()) return {};
    return it->second;
}

uint8_t RelationshipMgr::GetGender(uint32_t playerGuid) const {
    auto it = s_gender.find(playerGuid);
    if (it == s_gender.end()) return 0;
    return it->second;
}

void RelationshipMgr::SetGender(uint32_t playerGuid, uint8_t gender) {
    s_gender[playerGuid] = gender;
}

bool RelationshipMgr::CanFormRelationship(uint32_t playerGuid, uint32_t targetGuid, RelationshipType type) const {
    if (playerGuid == targetGuid) return false;
    if (HasActiveRelationship(playerGuid)) return false;
    if (HasActiveRelationship(targetGuid)) return false;

    uint8_t g1 = GetGender(playerGuid);
    uint8_t g2 = GetGender(targetGuid);

    if (type == RelationshipType::Couple || type == RelationshipType::Engaged || type == RelationshipType::Married) {
        if (g1 == g2) return false;
    }
    return true;
}

bool RelationshipMgr::IsCouple(uint32_t playerGuid, uint32_t targetGuid) const {
    auto it = s_relationships.find(playerGuid);
    if (it == s_relationships.end()) return false;
    for (const auto &r : it->second) {
        if (r.player2Guid == targetGuid && r.status == RelationshipStatus::Active &&
            (r.type == RelationshipType::Couple || r.type == RelationshipType::Engaged || r.type == RelationshipType::Married)) {
            return true;
        }
    }
    return false;
}

bool RelationshipMgr::IsMarried(uint32_t playerGuid, uint32_t targetGuid) const {
    auto it = s_relationships.find(playerGuid);
    if (it == s_relationships.end()) return false;
    for (const auto &r : it->second) {
        if (r.player2Guid == targetGuid && r.status == RelationshipStatus::Active &&
            r.type == RelationshipType::Married) {
            return true;
        }
    }
    return false;
}

} // namespace dm
