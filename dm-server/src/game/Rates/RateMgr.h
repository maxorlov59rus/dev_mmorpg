// RateMgr.h — Управление рейтоми сервера
#pragma once
#include <cstdint>
#include <string>

namespace dm {

struct ServerRates {
    float xpRate;
    float dropRate;
    float goldRate;
    float reputationRate;
    float honorRate;
    float craftingRate;
    float gatheringRate;
    float restXpRate;
    float questMoneyRate;
    float questXpRate;
    float pvpHonorRate;
    float spiritXpRate;
    float seasonalRewardRate;
    float mistyAnomalyRate;
    float autoPvPFaithRate;
    // Weekend rates
    bool weekendBonusEnabled;
    float weekendXPMultiplier;
    float weekendDropMultiplier;
    float weekendGoldMultiplier;
    float weekendReputationMultiplier;
    float weekendCraftingMultiplier;
    float weekendGatheringMultiplier;
    float weekendQuestMultiplier;
    float weekendSpiritXPMultiplier;
    int weekendDays[2]; // 0=Sunday, 6=Saturday
};

class RateMgr {
public:
    static RateMgr &Instance();

    // Инициализация из конфига
    void LoadFromConfig();

    // Получение рейтов
    float GetXPRate() const;
    float GetDropRate() const;
    float GetGoldRate() const;
    float GetReputationRate() const;
    float GetHonorRate() const;
    float GetCraftingRate() const;
    float GetGatheringRate() const;
    float GetRestXPRate() const;
    float GetQuestMoneyRate() const;
    float GetQuestXPRate() const;
    float GetPvPHonorRate() const;
    float GetSpiritXPRate() const;
    float GetSeasonalRewardRate() const;
    float GetMistyAnomalyRate() const;
    float GetAutoPvPFaithRate() const;

    // Установка рейтов (для GM команд)
    void SetXPRate(float rate);
    void SetDropRate(float rate);
    void SetGoldRate(float rate);
    void SetReputationRate(float rate);

    // Получить все рейты
    ServerRates GetAllRates() const;

    // Weekend rates
    bool IsWeekend() const;
    bool IsWeekendBonusEnabled() const;
    float GetWeekendXPMultiplier() const;
    float GetWeekendDropMultiplier() const;
    float GetWeekendGoldMultiplier() const;
    float GetWeekendReputationMultiplier() const;
    float GetWeekendCraftingMultiplier() const;
    float GetWeekendGatheringMultiplier() const;
    float GetWeekendQuestMultiplier() const;
    float GetWeekendSpiritXPMultiplier() const;

    // Effective rates (base * weekend if applicable)
    float GetEffectiveXPRate() const;
    float GetEffectiveDropRate() const;
    float GetEffectiveGoldRate() const;
    float GetEffectiveReputationRate() const;
    float GetEffectiveCraftingRate() const;
    float GetEffectiveGatheringRate() const;
    float GetEffectiveQuestMoneyRate() const;
    float GetEffectiveQuestXPRate() const;
    float GetEffectiveSpiritXPRate() const;

    // Применение рейта к значению
    uint32_t ApplyXPRate(uint32_t baseExp) const;
    uint32_t ApplyDropRate(uint32_t baseCount) const;
    uint32_t ApplyGoldRate(uint32_t baseGold) const;
    uint32_t ApplyReputationRate(uint32_t baseRep) const;
};

} // namespace dm