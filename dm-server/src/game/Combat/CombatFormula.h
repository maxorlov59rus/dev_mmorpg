// CombatFormula.h — Детерминированная формула урона (Q16.16 fixed-point)
// shared/Formulas — единый контракт сервер/клиент
#pragma once
#include <cstdint>

namespace DM {
namespace Combat {

// Q16.16 fixed-point: 1 единица = 1/65536
using Q16 = int32_t;
constexpr Q16 Q16_ONE = 1 << 16;
constexpr Q16 Q16_HALF = 1 << 15;

// Типы урона
enum DamageType : uint8_t {
    DAMAGE_PHYSICAL = 0,
    DAMAGE_MAGICAL = 1,
    DAMAGE_HOLY = 2,
    DAMAGE_SHADOW = 3,
    DAMAGE_FIRE = 4,
    DAMAGE_FROST = 5,
    DAMAGE_NATURE = 6,
    DAMAGE_COUNT = 7,
};

// Типы резистов
enum ResistType : uint8_t {
    RESIST_ARMOR = 0,     // физический
    RESIST_HOLY = 1,
    RESIST_SHADOW = 2,
    RESIST_FIRE = 3,
    RESIST_FROST = 4,
    RESIST_NATURE = 5,
    RESIST_COUNT = 6,
};

// Контекст урона
struct DamageContext {
    uint32_t spellId = 0;
    DamageType damageType = DAMAGE_PHYSICAL;
    Q16 baseDamage = 0;          // базовый урон спелла (Q16)
    Q16 statsMod = Q16_ONE;     // модификатор статов (1.0 = без изменений)
    bool isCrit = false;
    Q16 critMult = Q16_ONE * 2; // крит-множитель (2.0 по умолчанию)
    uint64_t seed = 0;          // seed для variance (server tick + guid)
    uint32_t attackerLevel = 1;
    uint32_t targetLevel = 1;
    Q16 targetArmor = 0;        // броня цели (Q16)
    Q16 schoolResist = Q16_ONE;  // резист школы (1.0 = без резиста)
};

// Кривая брони (armor_curve)
inline Q16 ArmorCurve(Q16 attackerIlvl, Q16 targetIlvl) {
    // Упрощённая кривая: чем выше ilvl атакующего, тем меньше снижение
    // Формула: 1.0 - (targetIlvl - attackerIlvl) * 0.01, clamp [0.5, 1.0]
    Q16 diff = targetIlvl - attackerIlvl;
    Q16 reduction = diff * (Q16_ONE / 100); // 1% per ilvl difference
    Q16 result = Q16_ONE - reduction;
    if (result < Q16_HALF) result = Q16_HALF; // min 50% mitigation
    if (result > Q16_ONE) result = Q16_ONE;   // max 100% (no bonus)
    return result;
}

// Вариация ±5% (детерминированная, seeded)
inline Q16 Variance(Q16 value, uint64_t seed) {
    // xorshift64* для детерминизма
    uint64_t x = seed;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    uint64_t hash = x * 0x2545F4914F6CDD1DULL;
    // Преобразуем в диапазон [-5%, +5%]
    int32_t var = (int32_t)(hash % 11) - 5; // -5..+5
    Q16 varQ16 = (Q16)var * (Q16_ONE / 100); // 1% = 0.01 in Q16
    return value + (value * varQ16 / Q16_ONE);
}

// Основная формула урона
inline Q16 CalculateDamage(const DamageContext &ctx) {
    // raw = base_spell * (1 + stats_mod) * crit ? crit_mult : 1
    Q16 raw = ctx.baseDamage;
    raw = (raw * ctx.statsMod) / Q16_ONE;
    if (ctx.isCrit) {
        raw = (raw * ctx.critMult) / Q16_ONE;
    }

    // mit = raw * armor_curve * school_resist
    Q16 armorMit = ArmorCurve(ctx.attackerLevel * Q16_ONE, ctx.targetLevel * Q16_ONE);
    Q16 mit = (raw * armorMit) / Q16_ONE;
    mit = (mit * ctx.schoolResist) / Q16_ONE;

    // final = clamp(mit * variance(±5% seeded), 1, ...)
    Q16 final = Variance(mit, ctx.seed);
    if (final < Q16_ONE) final = Q16_ONE; // min 1.0 damage

    return final;
}

// Конвертация Q16 в int для отображения
inline int32_t Q16ToInt(Q16 val) {
    return val >> 16;
}

// Конвертация int в Q16
inline Q16 IntToQ16(int32_t val) {
    return val << 16;
}

} // namespace Combat
} // namespace DM