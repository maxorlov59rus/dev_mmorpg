// BeginnerBuffMgr.h — Бафф новичка для игроков 1-10 уровня
#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

namespace dm {

struct BeginnerBuffStats {
    float xpMultiplier;
    float dropMultiplier;
    float goldMultiplier;
    float movementSpeed;
    float damageReduction;
};

class BeginnerBuffMgr {
public:
    static BeginnerBuffMgr &Instance();

    // Проверки
    bool IsBeginnerBuffActive(uint32_t playerGuid, uint8_t playerLevel) const;
    bool ShouldApplyBuff(uint8_t playerLevel) const;
    bool ShouldRemoveBuff(uint8_t playerLevel) const;

    // Получение статов
    BeginnerBuffStats GetBuffStats() const;
    float GetXPMultiplier(uint8_t playerLevel) const;
    float GetDropMultiplier(uint8_t playerLevel) const;
    float GetGoldMultiplier(uint8_t playerLevel) const;
    float GetMovementSpeedMultiplier(uint8_t playerLevel) const;
    float GetDamageReduction(uint8_t playerLevel) const;

    // Применение/снятие
    void ApplyBuff(uint32_t playerGuid, uint8_t playerLevel);
    void RemoveBuff(uint32_t playerGuid, uint8_t playerLevel);

    // Конфигурация
    bool IsEnabled() const;
    uint8_t GetLevelCap() const;
    bool IsAutoApply() const;
    bool ShowInUI() const;

    // Загрузка данных
    void LoadConfig();
};

} // namespace dm