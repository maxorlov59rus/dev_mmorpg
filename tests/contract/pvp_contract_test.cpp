// Кросс-контракт тест: enum PvpFlags (сервер) == биты defs/SQL == клиентские
// struct-параметры. Компилируется вместе с PvPSystem.cpp без UE-заголовков.
#include "game/PvP/PvPSystem.h"
#include <cassert>
#include <cstdio>
int main() {
    using namespace dm;
    static_assert(PVP_FLAGGED == 1 && FFA == 2 && IN_SANCTUARY == 4 && DUELING == 8 &&
                  WANTED == 16 && DISHONORED == 32, "bitmask contract");

    auto &pvp = PvPSystem::Instance();
    pvp.LoadZoneCache({{1, 101, "Dawnhold", RULESET_SANCTUARY, true, false, false, 65536},
                       {2, 201, "RustWastes", RULESET_FFA, false, false, false, 98304}});
    // Sanctuary: флаг не поднять
    pvp.OnEnterZone(10, 1, 101);
    assert(!pvp.OnPvpCommand(10, true));
    assert(pvp.GetFlags(10) & IN_SANCTUARY);
    // Выход в FFA: автофлаг зоны, атака разрешена всем
    pvp.OnEnterZone(10, 2, 201);
    assert(pvp.GetFlags(10) & FFA);
    assert(pvp.OnAttackPlayer(10, 20));
    assert(pvp.CanAttack(10, 20));
    // Убийство софракциона без флага -> rep -75 + dishonor при разрыве >10
    auto o = pvp.OnPlayerKilledPlayer(10, 20, 2, 201, true, false, false, 30, 15);
    assert(o.repDelta == -75 && o.dishonored && o.honorGain == 0);
    // Honor x1.5 в FFA за флагнутого
    auto o2 = pvp.OnPlayerKilledPlayer(10, 21, 2, 201, false, false, true, 30, 29);
    assert(o2.honorGain == 150 && o2.repDelta == 0);
    // SQL совпадает со схемой PvE_PvP_Core.sql
    std::string sql = pvp.BuildPersistSql(10);
    assert(sql.find("dm_world.character_flags") != std::string::npos &&
           sql.find("ON DUPLICATE KEY UPDATE") != std::string::npos);
    std::string log = pvp.BuildKillLogSql(o, 10, 20, 2, 201, 3, 1730000000);
    assert(log.find("dm_world.pvp_kills_log") != std::string::npos &&
           log.find("-75") != std::string::npos);
    // Дуэль: request->accept->complete снимает DUELING
    uint64_t id = pvp.DuelRequest(11, 12);
    assert(id && pvp.DuelAccept(id, true));
    assert(pvp.GetFlags(11) & DUELING);
    pvp.DuelComplete(id, 11, 0);
    assert(!(pvp.GetFlags(11) & DUELING));
    std::printf("PVP_CONTRACT_OK\n");
    return 0;
}
