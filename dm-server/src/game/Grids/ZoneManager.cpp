// dm-server/src/game/Grids/ZoneManager.cpp
// Реализация ядра карты локаций. Байтовый порядок пакетов — little-endian,
// как во всём протоколе «Чёрной Материи» (proto/opcodes.defs).
#include "Grids/ZoneManager.h"

#include "game/PvP/PvPSystem.h"

#include <algorithm>
#include <cstring>

namespace dm {
namespace {

void AppendU16(std::string &b, uint16_t v) {
    b.push_back(char(v & 0xFF));
    b.push_back(char((v >> 8) & 0xFF));
}
void AppendU8(std::string &b, uint8_t v) {
    b.push_back(char(v));
}

} // namespace

ZoneManager &ZoneManager::Instance() {
    static ZoneManager inst;
    return inst;
}

void ZoneManager::LoadZones(const std::vector<FZoneInfo> &rows) {
    m_zones.clear();
    m_zones.reserve(rows.size());
    for (const auto &r : rows)
        m_zones[ZoneKey(r.mapId, r.areaId)] = r;
}

void ZoneManager::LoadTeleports(const std::vector<FAreaTriggerTeleport> &rows) {
    m_teleports.clear();
    for (const auto &t : rows)
        m_teleports[t.triggerId] = t;
}

const FZoneInfo *ZoneManager::FindZone(uint16_t mapId, uint16_t areaId) const {
    auto it = m_zones.find(ZoneKey(mapId, areaId));
    return it == m_zones.end() ? nullptr : &it->second;
}

bool ZoneManager::OnPlayerEnterZone(uint32_t guid, uint16_t mapId, uint16_t areaId) {
    const uint32_t key = ZoneKey(mapId, areaId);
    auto it = m_current.find(guid);
    if (it != m_current.end() && it->second == key)
        return false; // без смены зоны — не спамим пакетами
    m_current[guid] = key;

    // 1) Правила PvP/PvE зоны применяет PvPSystem (источник контракта v1.7):
    //    Sanctuary -> SetPVPFlag(false, forced=true); FFA -> автофлаг всем;
    //    Arena -> resurrect/clean debuffs (см. PvpZoneInfo flags в kSQL ниже).
    PvPSystem::Instance().OnEnterZone(guid, mapId, areaId);

    // 2) TODO(KERNEL): session send SMSG_ZONE_UPDATE 0x204C (BuildZoneUpdatePacket)
    //    и повторная AOI-рассылка SMSG_PVP_FLAG_UPDATE для наблюдающих.
    return true;
}

uint16_t ZoneManager::GraveyardFor(uint16_t mapId, uint16_t areaId) const {
    const FZoneInfo *z = FindZone(mapId, areaId);
    return z ? z->graveyardMap : 0xFFFFu;
}

const FAreaTriggerTeleport *ZoneManager::FindTeleport(uint32_t triggerId) const {
    auto it = m_teleports.find(triggerId);
    return it == m_teleports.end() ? nullptr : &it->second;
}

bool ZoneManager::CanUseTeleport(uint8_t playerLevel, bool hasQuestRequired,
                                 const FAreaTriggerTeleport &tp) const {
    if (playerLevel < tp.minLevel)
        return false;
    if (tp.questRequired != 0 && !hasQuestRequired)
        return false; // гейт LFG-интро q_1105 для Пещер Испытаний (trigger 7001)
    // instance_lock_check сверяется выше слоем instance_locks (TODO(KERNEL)).
    return true;
}

std::string ZoneManager::BuildZoneMapPayload() const {
    std::string b;
    AppendU16(b, uint16_t(std::min<size_t>(m_zones.size(), 0xFFFF)));
    for (const auto &kv : m_zones) {
        const FZoneInfo &z = kv.second;
        AppendU16(b, z.mapId);
        AppendU16(b, z.areaId);
        AppendU8(b, uint8_t(z.category));
        AppendU8(b, z.minLevel);
        AppendU8(b, z.maxLevel);
        AppendU16(b, z.factionId);
        AppendU8(b, z.ruleset);
    }
    return b;
}

std::string ZoneManager::BuildZoneUpdatePacket(uint16_t mapId, uint16_t areaId) const {
    std::string b;
    AppendU16(b, mapId);
    AppendU16(b, areaId);
    const FZoneInfo *z = FindZone(mapId, areaId);
    AppendU8(b, z ? uint8_t(z->category) : 0);
    AppendU8(b, z ? z->minLevel : 1);
    AppendU8(b, z ? z->maxLevel : 95);
    AppendU16(b, z ? z->factionId : 0);
    return b;
}

// ---------------------------------------------------------------------------
// SQL-контракт загрузки (dm_world; строки соответствуют v_zone_map_export):
//   SELECT map_id, area_id, name, category, min_level, max_level, faction_id,
//          ruleset, honor_mult_q16, graveyard_map FROM v_zone_map_export;
//   SELECT trigger_id, from_map, from_area, to_map, to_area, to_x, to_y, to_z,
//          min_level, quest_required, instance_lock_check FROM areatrigger_teleport;
// Маппинг строк в FZoneInfo выполняет SqlResult-адаптер worldserver'а
// (TODO(KERNEL): WorldSession::StartZoneCacheLoad).
// ---------------------------------------------------------------------------

} // namespace dm
