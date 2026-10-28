// =====================================================================
// «Чёрная Материя» — ProgressionCore: математика прогрессии 1–95.
// Артефакт 1 промпта «Навыки/таланты/экипировка 1–95» (docs/09).
//
// ВСЕ ФОРМУЛЫ — целочисленные / fixed-point Q16 (ADR-3, docs/02 §6):
// никаких float на сервере, детерминизм между mapserver'ами.
//
//   BaseStat(L)  = ClassBase + RaceBonus + floor(Level * GrowthRateQ16 / 65536)
//   XP_Next(L)   = рекуррентно ceil(prev * (1 + L*7/100)), нормировано так,
//                  чтобы кумулятивно: L10~11.2k, L50~473k, L95~15.1M (cap 4M/lvl).
//   DR(P)        = табличный log10 (без std::log10) * CoeffQ8 — детерминизм.
//   Armor(ilvl)  = ilvl * 6 / 5            (ilvl * 1.2)
//   WeaponDPS    = ilvl * 4 / 5            (ilvl * 0.8), в Q8 dmg/sec
//
// Контракт ID совпадает с PlayerClassMgr.h (RaceId/ClassId) и клиентским
// MMOProtocol.h / Data/ProgressionDataTable.h.
// =====================================================================
#pragma once

#include <array>
#include <cstdint>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

namespace DM {

// ---- Масштабы проекта (docs/09 §0) ----
inline constexpr uint8 MAX_PLAYER_LEVEL = 95;         // кап игрока
inline constexpr uint8 MAX_CREATURE_LEVEL = 100;      // кап мобов
inline constexpr uint8 TALENT_UNLOCK_LEVEL = 10;      // 1 очко таланта начиная с L10
inline constexpr uint16 TOTAL_TALENT_POINTS = MAX_PLAYER_LEVEL - TALENT_UNLOCK_LEVEL + 1; // 86
inline constexpr uint8 PRESTIGE_MIN_LEVEL = 91;       // Row7: 3 очка за 1 талант
inline constexpr uint8 CLASS_COUNT_MAX = 7;           // контракт с PlayerClassMgr::CLASS_COUNT

// ---- Fixed-point helpers (Q16 / Q8) ----
namespace fx {
inline constexpr int32_t Q16_SCALE = 65536;
inline constexpr int32_t Q8_SCALE = 256;
} // namespace fx

// ---- Типы статов (порядок = wire-порядок SMSG_INITIALIZE_STATS) ----
enum StatType : uint8_t {
    STAT_STRENGTH = 0,
    STAT_AGILITY = 1,
    STAT_STAMINA = 2,
    STAT_INTELLECT = 3,
    STAT_SPIRIT = 4,
    STAT_COUNT = 5,
};

// Вторичные характеристики (DR-кривые, docs/09 §1.2)
enum SecondaryStat : uint8_t {
    SEC_CRIT = 0,      // рейтинг критического удара
    SEC_HASTE = 1,     // скорость атаки/каста
    SEC_MASTERY = 2,   // mastery (спец-эффект спеки)
    SEC_VERSATILE = 3, // универсальный %
    SEC_COUNT = 4,
};

// Классы брони (аналог item_template.ArmorType в TRC)
enum ArmorSubtype : uint8_t { ARMOR_CLOTH = 1, ARMOR_LEATHER = 2, ARMOR_MAIL = 3, ARMOR_PLATE = 4 };

// Тиры предметов (docs/09 §4.2)
enum ItemTier : uint8_t {
    TIER_COMMON = 0,   // серое/белое, оружие 1–20
    TIER_UNCOMMON = 1, // зеленое Dungeon Greens 1–60
    TIER_RARE = 2,     // синее, стихийный урон, оружие 50–90
    TIER_EPIC = 3,     // Raid Epics 60–80, вторички (Haste/Crit)
    TIER_MYTHIC = 4,   // Endgame 80–95, проки + сокеты
};

// ---- Таблица роста: ClassBase + GrowthRate по классам ----
struct ClassGrowthProfile {
    std::array<uint32, STAT_COUNT> Base{};      // статы на L1 (до расового бонуса)
    std::array<int32_t, STAT_COUNT> GrowthQ16{}; // прирост за уровень, Q16
    uint8 Power{1};                              // PowerType из PlayerClassMgr (Mana=1)
};

class ProgressionCore {
public:
    static const ProgressionCore& Instance();

    // ---------- Артефакт 1: базовые статы ----------
    // BaseStat = ClassBase + RaceBonus + floor((Level-1) * GrowthQ16 / 65536)
    uint32 ComputeBaseStat(uint8 classId, const std::array<uint32, STAT_COUNT>& raceBonus,
                           StatType stat, uint8 level) const;

    // Полный снапшот базовых статов уровня (SMSG_INITIALIZE_STATS / пересчет)
    std::array<uint32, STAT_COUNT> ComputeLevelStats(
        uint8 classId, const std::array<uint32, STAT_COUNT>& raceBonus, uint8 level) const;

    const ClassGrowthProfile& GetClassProfile(uint8 classId) const;

    // ---------- Вторичные статы: DR-кривая ----------
    // StatValue% = Coeff * log10(1 + Points); возврат в Q8 (1250 => 4.88%).
    int32_t ComputeSecondaryDR(SecondaryStat sec, uint32 pointsInvested) const;

    // Перевод агрегатов экипировки во вторички (1 AGI = 2 armor + 0.01% crit)
    struct DerivedFromGear {
        uint32 Armor = 0;   // AGI*2 + броня предметов
        int32_t CritQ8 = 0; // AGI*0.01% в Q8 + рейтинг crita
    };
    DerivedFromGear DeriveFromAgility(uint32 agility, uint32 critRating) const;

    // ---------- Кривая опыта ----------
    uint64 XpToNext(uint8 level) const;    // XP для перехода Level -> Level+1
    uint64 XpCumulative(uint8 level) const; // всего XP, чтобы ДОЙТИ до level

    // ---------- Экипировка: ilvl-бюджет ----------
    static uint32 ArmorBudget(uint32 ilvl) { return ilvl * 6 / 5; } // ilvl*1.2
    static uint32 WeaponDpsQ8(uint32 ilvl)                          // DPS=int(ilvl*0.8), Q8
    {
        return (ilvl * 4 / 5) * static_cast<uint32>(fx::Q8_SCALE);
    }
    // Слотный множитель брони в Q8 (нагрудник x2, голова/плечи/ноги x1.5)
    static uint32 ArmorSlotMultQ8(uint8 equipSlot);

    // Tier предмета по ilvl (генерация лута, валидация vendor'ов)
    static ItemTier TierForItemLevel(uint32 ilvl);

    bool IsValidLevel(uint8 level) const { return level >= 1 && level <= MAX_PLAYER_LEVEL; }

private:
    ProgressionCore();

    std::array<ClassGrowthProfile, CLASS_COUNT_MAX> Profiles{};
    std::array<uint64, MAX_PLAYER_LEVEL + 2> XpPrefix{}; // префиксные суммы XP
    std::array<int32_t, SEC_COUNT> SecCoeffQ8{};         // коэффициенты DR
    std::array<uint16, 1025> LogTable{};                 // log10(x)*256, x=1..1024
};

// ---- Расписание выдачи способностей (Артефакт 2, docs/09 §2) ----
enum AbilityCadence : uint8_t {
    CADENCE_CORE = 1,     // L1–10: каждые 2 уровня, 3–4 кнопки
    CADENCE_ROTATION = 2, // L11–40: каждые 3 уровня
    CADENCE_COOLDOWN = 3, // L41–65: каждые 5 уровней (кулдауны/утилити)
    CADENCE_BURST = 4,    // L66–90: «эйсы в рукаве», ~каждые 6
    CADENCE_PRESTIGE = 5, // L91–95: апгрейды существующих спеллов
};
AbilityCadence CadenceForLevel(uint8 level);
bool LevelGrantsSpell(uint8 level); // частотный фильтр поверх SQL class_spells

// ---- Валидация древа талантов (Артефакт 3) ----
struct TalentNodeRef {
    uint16 TreeId{0}; // 1..3 (ветки класса)
    uint8 Row{0};     // 1..7 (Row7 = prestige)
    uint8 Column{0};  // 1..4
};

// Стоимость вложения: Row7 prestige = 3 очка за 1 ранг, иначе 1.
uint8 TalentPointCost(const TalentNodeRef& node);

} // namespace DM
