// FactionRelationMgr.h
#pragma once
#include <cstdint>

namespace dm {

class FactionRelationMgr {
public:
    static FactionRelationMgr &Instance();
    void SetRelation(uint32_t factionA, uint32_t factionB, int8_t relation);
    int8_t GetRelation(uint32_t factionA, uint32_t factionB) const;
    bool IsHostile(uint32_t factionA, uint32_t factionB) const;
    bool IsAllied(uint32_t factionA, uint32_t factionB) const;
    void OnReputationChanged(uint32_t playerGuid, uint32_t faction, int32_t newStanding);
    bool CanAttack(uint32_t attackerFaction, uint32_t targetFaction) const;
};

} // namespace dm
