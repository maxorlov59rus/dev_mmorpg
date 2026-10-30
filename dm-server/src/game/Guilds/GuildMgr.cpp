#include "game/Guilds/GuildMgr.h"
#include <unordered_map>
#include <vector>

namespace dm {

static uint32_t s_nextGuildId = 1;
static std::unordered_map<uint32_t, std::vector<GuildMember>> s_guildMembers;
static std::unordered_map<uint32_t, uint32_t> s_playerGuild;

GuildMgr &GuildMgr::Instance() {
    static GuildMgr inst;
    return inst;
}

uint32_t GuildMgr::CreateGuild(uint32_t leaderGuid, const std::string &name) {
    uint32_t guildId = s_nextGuildId++;
    s_guildMembers[guildId].push_back({leaderGuid, 0, 0});
    s_playerGuild[leaderGuid] = guildId;
    (void)name;
    return guildId;
}

void GuildMgr::AddMember(uint32_t guildId, uint32_t playerGuid, uint8_t rank) {
    s_guildMembers[guildId].push_back({playerGuid, rank, 0});
    s_playerGuild[playerGuid] = guildId;
}

void GuildMgr::RemoveMember(uint32_t guildId, uint32_t playerGuid) {
    auto &members = s_guildMembers[guildId];
    for (auto it = members.begin(); it != members.end(); ++it) {
        if (it->playerGuid == playerGuid) {
            members.erase(it);
            break;
        }
    }
    s_playerGuild.erase(playerGuid);
}

void GuildMgr::SetRank(uint32_t guildId, uint32_t playerGuid, uint8_t rank) {
    for (auto &m : s_guildMembers[guildId]) {
        if (m.playerGuid == playerGuid) {
            m.rank = rank;
            break;
        }
    }
}

void GuildMgr::AddContribution(uint32_t guildId, uint32_t playerGuid, uint32_t amount) {
    for (auto &m : s_guildMembers[guildId]) {
        if (m.playerGuid == playerGuid) {
            m.contribution += amount;
            break;
        }
    }
}

uint32_t GuildMgr::GetGuildId(uint32_t playerGuid) const {
    auto it = s_playerGuild.find(playerGuid);
    if (it == s_playerGuild.end()) return 0;
    return it->second;
}

bool GuildMgr::IsMember(uint32_t guildId, uint32_t playerGuid) const {
    for (const auto &m : s_guildMembers.at(guildId)) {
        if (m.playerGuid == playerGuid) return true;
    }
    return false;
}

void GuildMgr::DisbandGuild(uint32_t guildId) {
    for (const auto &m : s_guildMembers[guildId]) {
        s_playerGuild.erase(m.playerGuid);
    }
    s_guildMembers.erase(guildId);
}

} // namespace dm
