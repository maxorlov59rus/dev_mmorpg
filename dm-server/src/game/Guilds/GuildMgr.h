// GuildMgr.h — гильдии
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace dm {

struct GuildMember {
    uint32_t playerGuid;
    uint8_t rank;
    uint32_t contribution;
};

class GuildMgr {
public:
    static GuildMgr &Instance();
    uint32_t CreateGuild(uint32_t leaderGuid, const std::string &name);
    void AddMember(uint32_t guildId, uint32_t playerGuid, uint8_t rank);
    void RemoveMember(uint32_t guildId, uint32_t playerGuid);
    void SetRank(uint32_t guildId, uint32_t playerGuid, uint8_t rank);
    void AddContribution(uint32_t guildId, uint32_t playerGuid, uint32_t amount);
    uint32_t GetGuildId(uint32_t playerGuid) const;
    bool IsMember(uint32_t guildId, uint32_t playerGuid) const;
    void DisbandGuild(uint32_t guildId);
};

} // namespace dm
