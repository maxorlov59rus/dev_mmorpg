// dm-server/src/game/PvP/PvPSystem.cpp
// Реализация: флаги/зоны/дуэли/репутация + генерация SQL для
// character_flags и pvp_kills_log (sql/upgrades/PvE_PvP_Core.sql).
#include "PvPSystem.h"

#include <cstdio>

namespace dm {

PvPSystem &PvPSystem::Instance() {
    static PvPSystem inst;
    return inst;
}

static uint64_t ZoneKey(uint16_t mapId, uint16_t areaId) {
    return (uint64_t(mapId) << 16) | uint64_t(areaId);
}

void PvPSystem::LoadZoneCache(const std::vector<PvpZoneInfo> &rows) {
    m_zones.clear();
    for (const auto &z : rows)
        m_zones[ZoneKey(z.mapId, z.areaId)] = z;
}

const PvpZoneInfo *PvPSystem::FindZone(uint16_t mapId, uint16_t areaId) const {
    auto it = m_zones.find(ZoneKey(mapId, areaId));
    return it == m_zones.end() ? nullptr : &it->second;
}

bool PvPSystem::OnPvpCommand(uint32_t guid, bool enable) {
    // TODO(KERNEL): определить текущую зону игрока через MapServer::GetPlayerArea.
    uint16_t flags = GetFlags(guid);
    if (enable && (flags & IN_SANCTUARY))
        return false; // в sanctuary war mode недоступен
    if (enable)
        flags |= PVP_FLAGGED;
    else
        flags &= ~PVP_FLAGGED;
    m_flags[guid] = flags;
    // SMSG_PVP_FLAG_UPDATE 0x2041 -> AOI-рассылка (TODO(KERNEL): session send)
    return true;
}

void PvPSystem::OnEnterZone(uint32_t guid, uint16_t mapId, uint16_t areaId) {
    const PvpZoneInfo *zone = FindZone(mapId, areaId);
    uint16_t flags = GetFlags(guid);
    flags &= ~(IN_SANCTUARY | FFA); // зональные биты пересчитываем
    if (!zone) {
        m_flags[guid] = flags;
        return; // вне PvP-зон — нейтрально
    }
    switch (zone->ruleset) {
    case RULESET_SANCTUARY:
        flags |= IN_SANCTUARY;
        if (zone->forcedUntag)
            flags &= ~PVP_FLAGGED; // SetPvPFlag(false, forced=true)
        break;
    case RULESET_ARENA:
        // auto queue + Resurrect() + RemoveDebuffs() — на стороне MapServer
        break;
    case RULESET_FFA:
        flags |= FFA; // любой атакуем, включая согильдийцев
        break;
    }
    m_flags[guid] = flags;
}

bool PvPSystem::OnAttackPlayer(uint32_t attackerGuid, uint32_t victimGuid) {
    // Атака по игроку автофлагует атакующего (если цель не в sanctuary).
    uint16_t vf = GetFlags(victimGuid);
    if (vf & IN_SANCTUARY)
        return false;
    uint16_t af = GetFlags(attackerGuid);
    if (!(af & (IN_SANCTUARY))) {
        af |= PVP_FLAGGED;
        m_flags[attackerGuid] = af;
    }
    return true;
}

int32_t PvPSystem::ComputeRepPenalty(bool sameFaction, bool victimRenegade, bool victimFlagged) const {
    // Ренегат: потерь нет (контракт ReputationMgr «при убийстве ренегата обе
    // фракции не теряют репутацию»). Враг фракции: без штрафа. Софракцион
    // флагнутый (честный PvP): без штрафа. Софракцион без флага: штраф.
    if (victimRenegade || !sameFaction || victimFlagged)
        return 0;
    return m_cfg.repLossSameFaction;
}

PvPSystem::KillOutcome PvPSystem::OnPlayerKilledPlayer(uint32_t killerGuid, uint32_t victimGuid,
                                                       uint16_t mapId, uint16_t areaId, bool sameFaction,
                                                       bool victimRenegade, bool victimFlagged,
                                                       uint8_t killerLevel, uint8_t victimLevel) {
    KillOutcome o;
    o.repDelta = ComputeRepPenalty(sameFaction, victimRenegade, victimFlagged);

    const PvpZoneInfo *zone = FindZone(mapId, areaId);
    uint32_t multQ16 = zone ? zone->honorMultQ16 : 65536u;
    // Honor только за «честные» цели: флагнутые/ренегаты/враги.
    bool honorEligible = victimRenegade || !sameFaction || victimFlagged;
    o.honorGain = honorEligible ? uint16_t((uint32_t(m_cfg.honorBaseKill) * multQ16) >> 16) : 0;

    // Dishonorable: убийство слабого софракциона (не флагнутого, не ренегата).
    if (sameFaction && !victimRenegade && !victimFlagged &&
        victimLevel + m_cfg.dishonorLevelGap < killerLevel)
        o.dishonored = true;

    // Wanted level: растёт за убийства нефлагнутых софракционов без своей смерти.
    if (sameFaction && !victimFlagged && !victimRenegade) {
        uint8_t w = ++m_wanted[killerGuid];
        if (w > 5)
            w = 5;
        m_wanted[killerGuid] = w;
        o.wantedLevel = w;
        if (w >= m_cfg.wantedKillsNoDeath)
            m_flags[killerGuid] = GetFlags(killerGuid) | WANTED;
    } else if (!sameFaction || victimFlagged) {
        o.wantedLevel = [&]() -> uint8_t {
            auto it = m_wanted.find(killerGuid);
            return it == m_wanted.end() ? 0 : it->second;
        }();
    }
    (void)victimGuid;
    return o;
}

uint64_t PvPSystem::DuelRequest(uint32_t initiator, uint32_t target) {
    if (initiator == target)
        return 0;
    uint16_t tf = GetFlags(target);
    if (tf & (IN_SANCTUARY | DUELING))
        return 0; // нельзя дуэль в sanctuary / уже в бою
    DuelState d{m_nextDuelId++, initiator, target, 0};
    m_duels[d.duelId] = d;
    // SMSG_DUEL_REQUEST -> цель; при accept: SMSG_DUEL_STARTED 0x2044
    return d.duelId;
}

bool PvPSystem::DuelAccept(uint64_t duelId, bool accept) {
    auto it = m_duels.find(duelId);
    if (it == m_duels.end())
        return false;
    if (!accept) {
        m_duels.erase(it);
        return false;
    }
    m_flags[it->second.initiatorGuid] |= DUELING;
    m_flags[it->second.targetGuid] |= DUELING;
    return true;
}

void PvPSystem::DuelComplete(uint64_t duelId, uint32_t winnerGuid, uint8_t reason) {
    auto it = m_duels.find(duelId);
    if (it == m_duels.end())
        return;
    // Снять DUELING у обоих; победителю — восстановить HP/MP и снять дебаффы
    // (реализует MapServer по SMSG_DUEL_COMPLETE 0x2046).
    for (uint32_t g : {it->second.initiatorGuid, it->second.targetGuid})
        m_flags[g] = GetFlags(g) & ~DUELING;
    (void)winnerGuid;
    (void)reason;
    m_duels.erase(it);
}

bool PvPSystem::CanAttack(uint32_t attackerGuid, uint32_t victimGuid) const {
    uint16_t vf = GetFlags(victimGuid);
    uint16_t af = GetFlags(attackerGuid);
    if (vf & IN_SANCTUARY)
        return false; // цель защищена городом
    if (af & IN_SANCTUARY)
        return false; // атакующий в sanctuary не может нападать
    if (vf & (FFA | PVP_FLAGGED | DUELING))
        return true;
    if ((af & DUELING) && (vf & DUELING))
        return true; // дуэлянты бьют только друг друга (проверка пары выше)
    return (af & PVP_FLAGGED) != 0;
}


uint8_t PvPSystem::GetWantedForSql(uint32_t guid) const {
    auto it = m_wanted.find(guid);
    return it == m_wanted.end() ? 0 : it->second;
}

std::string PvPSystem::BuildPersistSql(uint32_t guid) const {
    // character_flags: источник истины между сессиями (персист при logout/debounce).
    char buf[512];
    uint16_t f = GetFlags(guid);
    std::snprintf(buf, sizeof(buf),
                  "INSERT INTO dm_world.character_flags (guid,pvp_flags,honor_points,wanted_level)"
                  " VALUES (%u,%u,0,%u)"
                  " ON DUPLICATE KEY UPDATE pvp_flags=%u, wanted_level=%u",
                  guid, unsigned(f), unsigned(GetWantedForSql(guid)), unsigned(f),
                  unsigned(GetWantedForSql(guid)));
    return std::string(buf);
}

std::string PvPSystem::BuildKillLogSql(const KillOutcome &o, uint32_t killer, uint32_t victim,
                                       uint16_t mapId, uint16_t areaId, uint8_t ruleset,
                                       uint32_t ts) const {
    char buf[512];
    std::snprintf(buf, sizeof(buf),
                  "INSERT INTO dm_world.pvp_kills_log"
                  " (killer_guid,victim_guid,map_id,area_id,zone_ruleset,rep_delta,honor_gain,dishonored,timestamp)"
                  " VALUES (%u,%u,%u,%u,%u,%d,%u,%u,%u)",
                  killer, victim, mapId, areaId, unsigned(ruleset), int(o.repDelta),
                  unsigned(o.honorGain), unsigned(o.dishonored ? 1u : 0u), ts);
    return std::string(buf);
}

} // namespace dm
