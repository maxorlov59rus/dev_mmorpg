#include "game/Factions/FactionRelationMgr.h"
#include <map>

namespace dm {

FactionRelationMgr &FactionRelationMgr::Instance() {
    static FactionRelationMgr inst;
    return inst;
}

static std::map<std::pair<uint32_t, uint32_t>, int8_t> s_relations;

void FactionRelationMgr::SetRelation(uint32_t factionA, uint32_t factionB, int8_t relation) {
    s_relations[{factionA, factionB}] = relation;
    s_relations[{factionB, factionA}] = relation;
}

int8_t FactionRelationMgr::GetRelation(uint32_t factionA, uint32_t factionB) const {
    auto it = s_relations.find({factionA, factionB});
    if (it == s_relations.end())
        return 0;
    return it->second;
}

bool FactionRelationMgr::IsHostile(uint32_t factionA, uint32_t factionB) const {
    return GetRelation(factionA, factionB) <= -1;
}

bool FactionRelationMgr::IsAllied(uint32_t factionA, uint32_t factionB) const {
    return GetRelation(factionA, factionB) >= 1;
}

void FactionRelationMgr::OnReputationChanged(uint32_t playerGuid, uint32_t faction,
                                             int32_t newStanding) {
    if (newStanding < -3000) {
        // SetRelation(faction, FACTION_PLAYER, -2);
    }
}

bool FactionRelationMgr::CanAttack(uint32_t attackerFaction, uint32_t targetFaction) const {
    return IsHostile(attackerFaction, targetFaction);
}

} // namespace dm
