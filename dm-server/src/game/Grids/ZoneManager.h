// dm-server/src/game/Grids/ZoneManager.h
// Ядро карты локаций: кэш areatable/map/pvp_zones/graveyard_binding из dm_world
// (sql/create/06_world_map.sql). Источник истины для ZoneId/AreaId игрока;
// связан с PvPSystem::OnEnterZone (флаги Sanctuary/FFA/Arena) и PvEEncounterMgr
// (graveyard при смерти в PvE-зоне).
//
// Пакеты: CMSG_ZONE_MAP_REQUEST 0x204A -> SMSG_ZONE_MAP 0x204B (полный снимок),
//         SMSG_ZONE_UPDATE 0x204C (смена зоны, map-канал).
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace dm {

/// areatable.category — совпадает с EZoneCategory клиента (UI_MapWidget).
enum ZoneCategory : uint8_t {
    ZCAT_CITY = 1,      // Sanctuary-город: рамка миникарты зелёная
    ZCAT_PVE_ZONE = 2,  // обычная PvE-зона
    ZCAT_FFA = 3,       // Free For All: рамка красная
    ZCAT_ARENA = 4,     // инстанс-арена
    ZCAT_DUNGEON = 5,   // подземелье (LFG)
    ZCAT_RAID = 6,      // рейд
    ZCAT_WILDERNESS = 7 // спорная зона (PvP возможен по флагу War Mode)
};

/// Одна локация карты (строка v_zone_map_export).
struct FZoneInfo {
    uint16_t mapId = 0;
    uint16_t areaId = 0; // == areatable.zone_id == pvp_zones.area_id
    std::string name;
    ZoneCategory category = ZCAT_PVE_ZONE;
    uint8_t minLevel = 1;
    uint8_t maxLevel = 95;
    uint16_t factionId = 0; // 0 neutral / 1 Dawn / 2 Ember
    uint8_t ruleset = 0;    // 0 нет записи pvp_zones / 1 Sanctuary / 2 Arena / 3 FFA
    uint32_t honorMultQ16 = 65536u;
    uint16_t graveyardMap = 0xFFFF; // 0xFFFF = нет привязки
};

/// Точка телепорта areatrigger_teleport.
struct FAreaTriggerTeleport {
    uint32_t triggerId = 0;
    uint16_t fromMap = 0, fromArea = 0, toMap = 0, toArea = 0;
    float x = 0.f, y = 0.f, z = 0.f;
    uint8_t minLevel = 1;
    uint32_t questRequired = 0; // q_1105 gate для Пещер Испытаний
    bool instanceLockCheck = false;
};

class ZoneManager {
public:
    static ZoneManager &Instance();

    /// SELECT * FROM v_zone_map_export — вызывается при старте worldserver/map.
    void LoadZones(const std::vector<FZoneInfo> &rows);
    /// SELECT * FROM areatrigger_teleport.
    void LoadTeleports(const std::vector<FAreaTriggerTeleport> &rows);

    const FZoneInfo *FindZone(uint16_t mapId, uint16_t areaId) const;
    uint32_t ZoneKey(uint16_t mapId, uint16_t areaId) const { return (uint32_t(mapId) << 16) | areaId; }

    /// Хук перемещения игрока (MapServer::MovePlayer / teleport). Возвращает true,
    /// если зона реально сменилась. Внутри:
    ///  1) обновляет m_current[guid];
    ///  2) дергает PvPSystem::OnEnterZone (Sanctuary force-untag / FFA автофлаг);
    ///  3) формирует payload SMSG_ZONE_UPDATE 0x204C (см. BuildZoneUpdatePacket).
    bool OnPlayerEnterZone(uint32_t guid, uint16_t mapId, uint16_t areaId);

    /// Кладбище для политики смерти PvE (PvEEncounterMgr::OnPlayerDied).
    uint16_t GraveyardFor(uint16_t mapId, uint16_t areaId) const;

    /// Гейт входа в инстанс/арену: уровень + квест (q_1105) + lockout-check флаг.
    const FAreaTriggerTeleport *FindTeleport(uint32_t triggerId) const;
    bool CanUseTeleport(uint8_t playerLevel, bool hasQuestRequired,
                        const FAreaTriggerTeleport &tp) const;

    /// Снимок всей карты для SMSG_ZONE_MAP 0x204B: count + записи
    /// (u16 map,u16 area,u8 cat,u8 lmin,u8 lmax,u16 faction,u8 ruleset).
    std::string BuildZoneMapPayload() const;
    /// Один апдейт для SMSG_ZONE_UPDATE 0x204C.
    std::string BuildZoneUpdatePacket(uint16_t mapId, uint16_t areaId) const;

    size_t ZoneCount() const { return m_zones.size(); }

private:
    ZoneManager() = default;
    std::unordered_map<uint32_t, FZoneInfo> m_zones; // key = map<<16|area
    std::unordered_map<uint32_t, FAreaTriggerTeleport> m_teleports;
    std::unordered_map<uint32_t, uint32_t> m_current; // guid -> zone key
};

} // namespace dm
