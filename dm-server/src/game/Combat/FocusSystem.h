// FocusSystem.h — Система Focus (комбо-очки) + вторичные ресурсы
// GDD §2: Focus 0-4 + уникальный второй ресурс ветки
#pragma once
#include <cstdint>

namespace DM {
namespace Combat {

// Типы вторичных ресурсов
enum SecondaryResourceType : uint8_t {
    RESOURCE_NONE = 0,
    RESOURCE_LUMEN = 1,      // Орден Рассвета (Светность)
    RESOURCE_UMBRA = 2,      // Круг Поглощения (Тень)
    RESOURCE_SPLINTERS = 3,  // Гильдия Порога (Осколки)
    RESOURCE_MANA = 4,       // Базовый ресурс (Жрец, Маг, Чернокнижник)
    RESOURCE_RAGE = 5,       // Воин
    RESOURCE_ENERGY = 6,     // Разбойник, Охотник
};

// Состояние Focus игрока
struct FocusState {
    uint8_t focusPoints = 0;     // 0-4 комбо-очков
    uint8_t maxFocus = 4;        // максимум комбо-очков
    SecondaryResourceType secondaryType = RESOURCE_NONE;
    int32_t secondaryAmount = 0;  // текущее количество вторичного ресурса
    int32_t secondaryMax = 100;   // максимум вторичного ресурса

    // Добавить Focus
    bool AddFocus(uint8_t amount) {
        if (focusPoints >= maxFocus) return false;
        focusPoints = (uint8_t)std::min((int)focusPoints + (int)amount, (int)maxFocus);
        return true;
    }

    // Потратить Focus
    bool SpendFocus(uint8_t amount) {
        if (focusPoints < amount) return false;
        focusPoints -= amount;
        return true;
    }

    // Добавить вторичный ресурс
    bool AddSecondary(int32_t amount) {
        if (secondaryType == RESOURCE_NONE) return false;
        secondaryAmount = std::min(secondaryAmount + amount, secondaryMax);
        return true;
    }

    // Потратить вторичный ресурс
    bool SpendSecondary(int32_t amount) {
        if (secondaryAmount < amount) return false;
        secondaryAmount -= amount;
        return true;
    }

    // Сброс Focus (при смене цели, смерти, etc.)
    void Reset() {
        focusPoints = 0;
        // Вторичный ресурс НЕ сбрасывается (кроме Rage/Energy)
        if (secondaryType == RESOURCE_RAGE || secondaryType == RESOURCE_ENERGY) {
            secondaryAmount = 0;
        }
    }
};

// Способность с требованием Focus
struct AbilityRequirement {
    uint8_t requiredFocus = 0;       // минимальный Focus для использования
    uint8_t focusCost = 0;           // стоимость Focus
    int32_t secondaryCost = 0;       // стоимость вторичного ресурса
    bool generatesFocus = false;      // генерирует ли Focus
    uint8_t focusGenerated = 0;      // сколько Focus генерирует
    bool generatesSecondary = false;  // генерирует ли вторичный ресурс
    int32_t secondaryGenerated = 0;  // сколько вторичного ресурса генерирует
};

// Проверка доступности способности
inline bool CanUseAbility(const FocusState &state, const AbilityRequirement &req) {
    if (state.focusPoints < req.requiredFocus) return false;
    if (state.secondaryAmount < req.secondaryCost) return false;
    return true;
}

// Применение способности (обновление состояния)
inline void ApplyAbility(FocusState &state, const AbilityRequirement &req) {
    state.SpendFocus(req.focusCost);
    state.SpendSecondary(req.secondaryCost);
    if (req.generatesFocus) state.AddFocus(req.focusGenerated);
    if (req.generatesSecondary) state.AddSecondary(req.secondaryGenerated);
}

} // namespace Combat
} // namespace DM