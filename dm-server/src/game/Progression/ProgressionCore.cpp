// =====================================================================
// «Чёрная Материя» — ProgressionCore.cpp (см. ProgressionCore.h)
// Детерминированная математика 1–95: статы, XP, DR, ilvl-бюджет.
// =====================================================================
#include "ProgressionCore.h"

#include <algorithm>
#include <cmath> // только для constexpr-инициализации таблицы log10 на старте

namespace DM {

// ---------------------------------------------------------------------
// Growth-профили классов (docs/09 §1.1). Воин: STR +3/lvl, INT +0.5/lvl...
// Значения Base = L1 из playercreateinfo_classlevelstats (race=Human baseline).
// ---------------------------------------------------------------------
// Индексы профилей совпадают с ClassId enum (PlayerClassMgr.h); держим
// локальные алиасы, чтобы .cpp не тянул весь заголовок классов.
enum : uint8_t {
    CLASS_WARRIOR_IDX = 1,
    CLASS_PRIEST_IDX = 2,
    CLASS_MAGE_IDX = 3,
    CLASS_ROGUE_IDX = 4,
    CLASS_HUNTER_IDX = 5,
    CLASS_WARLOCK_IDX = 6,
};

static constexpr int32_t GQ(int32_t perLevelTimes10)
{
    // прирост за уровень в Q16; perLevelTimes10 = value*10 для точности 0.x
    return static_cast<int32_t>(perLevelTimes10 * fx::Q16_SCALE / 10);
}

ProgressionCore::ProgressionCore()
{
    // class index = ClassId (1..6), [0] не используется
    Profiles[CLASS_WARRIOR_IDX] = {{22, 12, 20, 6, 8}, {GQ(30), GQ(15), GQ(25), GQ(5), GQ(7)}, 2};
    Profiles[CLASS_PRIEST_IDX] = {{8, 10, 13, 22, 24}, {GQ(8), GQ(10), GQ(15), GQ(30), GQ(28)}, 1};
    Profiles[CLASS_MAGE_IDX] = {{8, 14, 12, 24, 18}, {GQ(6), GQ(14), GQ(12), GQ(32), GQ(24)}, 1};
    Profiles[CLASS_ROGUE_IDX] = {{12, 24, 15, 8, 9}, {GQ(12), GQ(32), GQ(16), GQ(4), GQ(6)}, 3};
    Profiles[CLASS_HUNTER_IDX] = {{14, 22, 16, 7, 8}, {GQ(14), GQ(30), GQ(18), GQ(6), GQ(8)}, 3};
    Profiles[CLASS_WARLOCK_IDX] = {{10, 13, 14, 26, 12}, {GQ(8), GQ(12), GQ(18), GQ(30), GQ(16)}, 1};

    // Коэффициенты DR-кривых в Q8 (docs/09 §1.2): crit растёт медленнее haste.
    SecCoeffQ8[SEC_CRIT] = 768;      // ~3% на log10(1+P)=1 (P=9 очков)
    SecCoeffQ8[SEC_HASTE] = 640;     // 2.5%
    SecCoeffQ8[SEC_MASTERY] = 512;   // 2%
    SecCoeffQ8[SEC_VERSATILE] = 448; // 1.75%

    // Таблица log10(x)*256, x=0..1024 (x=0 => 0). Строится один раз; значения
    // целые и одинаковы на всех платформах (double->int после округления).
    for (uint32 i = 0; i < LogTable.size(); ++i)
    {
        const double v = (i == 0) ? 0.0 : std::log10(static_cast<double>(i));
        LogTable[i] = static_cast<uint16>(std::llround(v * 256.0));
    }

    // ---- Кривая опыта (docs/09 §1.3): XP_step(L+1) = ceil(XP_step(L) * 1.08)
    // Постоянный темп 8% подобран под целевые ориентиры кумулятивного XP:
    //   cum(L10)=11.2k (~10k), cum(L50)=473k (~500k), cum(L95)=15.1M (~15M).
    // Кап шага 4M XP/уровень — anti-runaway для балансировки эндгейма.
    uint64 running = 0;
    XpPrefix[1] = 0; // дожить до L1 ничего не нужно
    uint64 step = 900; // XP для перехода L1 -> L2
    for (uint32 l = 1; l <= MAX_PLAYER_LEVEL; ++l)
    {
        XpPrefix[l] = running;
        running += step;
        step = (step * 10800ULL + 9999ULL) / 10000ULL; // ceil(step*1.08)
        if (step > 4000000ULL)                         // cap эндгейм-шага
            step = 4000000ULL;
    }
    XpPrefix[MAX_PLAYER_LEVEL + 1] = running;
}

const ProgressionCore& ProgressionCore::Instance()
{
    static const ProgressionCore Core;
    return Core;
}

const ClassGrowthProfile& ProgressionCore::GetClassProfile(uint8 classId) const
{
    static const ClassGrowthProfile Fallback{};
    if (classId == 0 || classId >= CLASS_COUNT_MAX)
        return Fallback;
    return Profiles[classId];
}

uint32 ProgressionCore::ComputeBaseStat(uint8 classId, const std::array<uint32, STAT_COUNT>& raceBonus,
                                        StatType stat, uint8 level) const
{
    if (stat >= STAT_COUNT)
        return 0;
    const ClassGrowthProfile& P = GetClassProfile(classId);
    const uint8 lvl = std::max<uint8>(1, std::min(level, MAX_PLAYER_LEVEL));
    // floor((lvl-1) * GrowthQ16 / 65536) — целочисленно, без float
    const int64_t growth = (static_cast<int64_t>(lvl - 1) * P.GrowthQ16[stat]) >> 16;
    const int64_t total = static_cast<int64_t>(P.Base[stat]) + static_cast<int64_t>(raceBonus[stat]) + growth;
    return static_cast<uint32>(std::max<int64_t>(1, total));
}

std::array<uint32, STAT_COUNT> ProgressionCore::ComputeLevelStats(
    uint8 classId, const std::array<uint32, STAT_COUNT>& raceBonus, uint8 level) const
{
    std::array<uint32, STAT_COUNT> Out{};
    for (uint8 s = 0; s < STAT_COUNT; ++s)
        Out[s] = ComputeBaseStat(classId, raceBonus, static_cast<StatType>(s), level);
    return Out;
}

int32_t ProgressionCore::ComputeSecondaryDR(SecondaryStat sec, uint32 pointsInvested) const
{
    if (sec >= SEC_COUNT)
        return 0;
    // log10(1+P): табличный доступ, линейная экстраполяция за пределами 1024
    const uint64 idx64 = static_cast<uint64>(pointsInvested) + 1ULL;
    uint32 logQ8 = 0;
    if (idx64 < LogTable.size())
    {
        logQ8 = LogTable[static_cast<size_t>(idx64)];
    }
    else
    {
        // log10(x) = log10(x/1024) + log10(1024) ~= табличное + 3.01 шагов по 2
        uint64 x = idx64;
        uint32 doublings = 0;
        while (x >= LogTable.size())
        {
            x >>= 1;
            ++doublings;
        }
        const uint32 log2Q8 = 77u; // log10(2)*256 = 77.06 -> 77
        logQ8 = LogTable[static_cast<size_t>(x)] + doublings * log2Q8;
    }
    return static_cast<int32_t>((static_cast<int64_t>(SecCoeffQ8[sec]) * logQ8) >> 8);
}

ProgressionCore::DerivedFromGear ProgressionCore::DeriveFromAgility(uint32 agility, uint32 critRating) const
{
    DerivedFromGear D;
    D.Armor = agility * 2u; // 1 AGI = 2 брони
    // 1 AGI = 0.01% crit => crit%Q8 = agility * 256 / 10000 (целочисленно)
    D.CritQ8 = static_cast<int32_t>(critRating + (static_cast<uint64>(agility) * 256ull) / 10000ull);
    return D;
}

uint64 ProgressionCore::XpToNext(uint8 level) const
{
    if (level >= MAX_PLAYER_LEVEL)
        return 0; // кап
    return XpPrefix[level + 1] - XpPrefix[level];
}

uint64 ProgressionCore::XpCumulative(uint8 level) const
{
    if (level == 0 || level > MAX_PLAYER_LEVEL + 1)
        return 0;
    return XpPrefix[level];
}

uint32 ProgressionCore::ArmorSlotMultQ8(uint8 equipSlot)
{
    // CONTRACT equipSlot: 1=head 2=shoulders 3=chest 4=pants 5=hands 6=feet ...
    switch (equipSlot)
    {
    case 3: return 2u * fx::Q8_SCALE;                 // грудь x2.0
    case 1:
    case 2:
    case 4: return (fx::Q8_SCALE * 3u) / 2u;          // голова/плечи/ноги x1.5
    case 5:
    case 6: return fx::Q8_SCALE;                      // руки/обувь x1.0
    default: return fx::Q8_SCALE;
    }
}

ItemTier ProgressionCore::TierForItemLevel(uint32 ilvl)
{
    if (ilvl >= 80) return TIER_MYTHIC; // Endgame 80–95
    if (ilvl >= 60) return TIER_EPIC;   // Raid Epics 60–80
    if (ilvl >= 50) return TIER_RARE;   // синее
    if (ilvl >= 20) return TIER_UNCOMMON;
    return TIER_COMMON;
}

AbilityCadence CadenceForLevel(uint8 level)
{
    if (level <= 10) return CADENCE_CORE;
    if (level <= 40) return CADENCE_ROTATION;
    if (level <= 65) return CADENCE_COOLDOWN;
    if (level <= 90) return CADENCE_BURST;
    return CADENCE_PRESTIGE;
}

bool LevelGrantsSpell(uint8 level)
{
    if (level > MAX_PLAYER_LEVEL)
        return false;
    switch (CadenceForLevel(level))
    {
    case CADENCE_CORE:     return level == 1 || level % 2 == 1; // 1,3,5,7,9 (+10 спец)
    case CADENCE_ROTATION: return level % 3 == 2;               // 11,14,...,38
    case CADENCE_COOLDOWN: return level % 5 == 1;               // 41,46,...,61(+65)
    case CADENCE_BURST:    return level % 6 == 0 || level == 90;// 66,72,...,90
    case CADENCE_PRESTIGE: return true;                         // 91..95 каждый
    }
    return false;
}

uint8 TalentPointCost(const TalentNodeRef& node)
{
    // Row7 prestige таланты стоят 3 очка за 1 ранг (docs/09 §3)
    return (node.Row >= 7) ? 3 : 1;
}

} // namespace DM
