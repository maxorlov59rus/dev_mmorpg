#include "game/Rates/RateMgr.h"
#include "common/config/Config.h"
#include "game/Time/RealTimeMgr.h"
#include <ctime>

namespace dm {

static ServerRates s_rates = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    true, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, {0, 6}
};

RateMgr &RateMgr::Instance() {
    static RateMgr inst;
    return inst;
}

void RateMgr::LoadFromConfig() {
    auto &cfg = dm::common::config::Config::Instance();
    auto getFloat = [&cfg](const std::string &section, const std::string &key, float def) -> float {
        std::string val = cfg.GetString(section, key, "");
        if (val.empty()) return def;
        try { return std::stof(val); } catch (...) { return def; }
    };
    s_rates.xpRate = getFloat("Rates", "XPRate", 1.0f);
    s_rates.dropRate = getFloat("Rates", "DropRate", 1.0f);
    s_rates.goldRate = getFloat("Rates", "GoldRate", 1.0f);
    s_rates.reputationRate = getFloat("Rates", "ReputationRate", 1.0f);
    s_rates.honorRate = getFloat("Rates", "HonorRate", 1.0f);
    s_rates.craftingRate = getFloat("Rates", "CraftingRate", 1.0f);
    s_rates.gatheringRate = getFloat("Rates", "GatheringRate", 1.0f);
    s_rates.restXpRate = getFloat("Rates", "RestXPRate", 2.0f);
    s_rates.questMoneyRate = getFloat("Rates", "QuestMoneyRate", 1.0f);
    s_rates.questXpRate = getFloat("Rates", "QuestXPRate", 1.0f);
    s_rates.pvpHonorRate = getFloat("Rates", "PvPHonorRate", 1.0f);
    s_rates.spiritXpRate = getFloat("Rates", "SpiritXPRate", 1.0f);
    s_rates.seasonalRewardRate = getFloat("Rates", "SeasonalRewardRate", 1.0f);
    s_rates.mistyAnomalyRate = getFloat("Rates", "MistyAnomalyRate", 1.0f);
    s_rates.autoPvPFaithRate = getFloat("Rates", "AutoPvPFaithRate", 1.0f);

    // Weekend rates
    s_rates.weekendBonusEnabled = cfg.GetBool("WeekendRates", "WeekendBonusEnabled", true);
    s_rates.weekendXPMultiplier = getFloat("WeekendRates", "WeekendXPMultiplier", 2.0f);
    s_rates.weekendDropMultiplier = getFloat("WeekendRates", "WeekendDropMultiplier", 2.0f);
    s_rates.weekendGoldMultiplier = getFloat("WeekendRates", "WeekendGoldMultiplier", 2.0f);
    s_rates.weekendReputationMultiplier = getFloat("WeekendRates", "WeekendReputationMultiplier", 2.0f);
    s_rates.weekendCraftingMultiplier = getFloat("WeekendRates", "WeekendCraftingMultiplier", 2.0f);
    s_rates.weekendGatheringMultiplier = getFloat("WeekendRates", "WeekendGatheringMultiplier", 2.0f);
    s_rates.weekendQuestMultiplier = getFloat("WeekendRates", "WeekendQuestMultiplier", 2.0f);
    s_rates.weekendSpiritXPMultiplier = getFloat("WeekendRates", "WeekendSpiritXPMultiplier", 2.0f);
    s_rates.weekendDays[0] = 0; // Sunday
    s_rates.weekendDays[1] = 6; // Saturday
}

// Weekend methods
bool RateMgr::IsWeekend() const {
    if (!s_rates.weekendBonusEnabled) return false;
    return dm::RealTimeMgr::Instance().IsWeekend();
}

bool RateMgr::IsWeekendBonusEnabled() const { return s_rates.weekendBonusEnabled; }
float RateMgr::GetWeekendXPMultiplier() const { return s_rates.weekendXPMultiplier; }
float RateMgr::GetWeekendDropMultiplier() const { return s_rates.weekendDropMultiplier; }
float RateMgr::GetWeekendGoldMultiplier() const { return s_rates.weekendGoldMultiplier; }
float RateMgr::GetWeekendReputationMultiplier() const { return s_rates.weekendReputationMultiplier; }
float RateMgr::GetWeekendCraftingMultiplier() const { return s_rates.weekendCraftingMultiplier; }
float RateMgr::GetWeekendGatheringMultiplier() const { return s_rates.weekendGatheringMultiplier; }
float RateMgr::GetWeekendQuestMultiplier() const { return s_rates.weekendQuestMultiplier; }
float RateMgr::GetWeekendSpiritXPMultiplier() const { return s_rates.weekendSpiritXPMultiplier; }

float RateMgr::GetEffectiveXPRate() const {
    float rate = s_rates.xpRate;
    if (IsWeekend()) rate *= s_rates.weekendXPMultiplier;
    return rate;
}

float RateMgr::GetEffectiveDropRate() const {
    float rate = s_rates.dropRate;
    if (IsWeekend()) rate *= s_rates.weekendDropMultiplier;
    return rate;
}

float RateMgr::GetEffectiveGoldRate() const {
    float rate = s_rates.goldRate;
    if (IsWeekend()) rate *= s_rates.weekendGoldMultiplier;
    return rate;
}

float RateMgr::GetEffectiveReputationRate() const {
    float rate = s_rates.reputationRate;
    if (IsWeekend()) rate *= s_rates.weekendReputationMultiplier;
    return rate;
}

float RateMgr::GetEffectiveCraftingRate() const {
    float rate = s_rates.craftingRate;
    if (IsWeekend()) rate *= s_rates.weekendCraftingMultiplier;
    return rate;
}

float RateMgr::GetEffectiveGatheringRate() const {
    float rate = s_rates.gatheringRate;
    if (IsWeekend()) rate *= s_rates.weekendGatheringMultiplier;
    return rate;
}

float RateMgr::GetEffectiveQuestMoneyRate() const {
    float rate = s_rates.questMoneyRate;
    if (IsWeekend()) rate *= s_rates.weekendQuestMultiplier;
    return rate;
}

float RateMgr::GetEffectiveQuestXPRate() const {
    float rate = s_rates.questXpRate;
    if (IsWeekend()) rate *= s_rates.weekendQuestMultiplier;
    return rate;
}

float RateMgr::GetEffectiveSpiritXPRate() const {
    float rate = s_rates.spiritXpRate;
    if (IsWeekend()) rate *= s_rates.weekendSpiritXPMultiplier;
    return rate;
}

float RateMgr::GetXPRate() const { return s_rates.xpRate; }
float RateMgr::GetDropRate() const { return s_rates.dropRate; }
float RateMgr::GetGoldRate() const { return s_rates.goldRate; }
float RateMgr::GetReputationRate() const { return s_rates.reputationRate; }
float RateMgr::GetHonorRate() const { return s_rates.honorRate; }
float RateMgr::GetCraftingRate() const { return s_rates.craftingRate; }
float RateMgr::GetGatheringRate() const { return s_rates.gatheringRate; }
float RateMgr::GetRestXPRate() const { return s_rates.restXpRate; }
float RateMgr::GetQuestMoneyRate() const { return s_rates.questMoneyRate; }
float RateMgr::GetQuestXPRate() const { return s_rates.questXpRate; }
float RateMgr::GetPvPHonorRate() const { return s_rates.pvpHonorRate; }
float RateMgr::GetSpiritXPRate() const { return s_rates.spiritXpRate; }
float RateMgr::GetSeasonalRewardRate() const { return s_rates.seasonalRewardRate; }
float RateMgr::GetMistyAnomalyRate() const { return s_rates.mistyAnomalyRate; }
float RateMgr::GetAutoPvPFaithRate() const { return s_rates.autoPvPFaithRate; }

void RateMgr::SetXPRate(float rate) { s_rates.xpRate = rate; }
void RateMgr::SetDropRate(float rate) { s_rates.dropRate = rate; }
void RateMgr::SetGoldRate(float rate) { s_rates.goldRate = rate; }
void RateMgr::SetReputationRate(float rate) { s_rates.reputationRate = rate; }

ServerRates RateMgr::GetAllRates() const { return s_rates; }

uint32_t RateMgr::ApplyXPRate(uint32_t baseExp) const {
    return static_cast<uint32_t>(baseExp * s_rates.xpRate);
}

uint32_t RateMgr::ApplyDropRate(uint32_t baseCount) const {
    return static_cast<uint32_t>(baseCount * s_rates.dropRate);
}

uint32_t RateMgr::ApplyGoldRate(uint32_t baseGold) const {
    return static_cast<uint32_t>(baseGold * s_rates.goldRate);
}

uint32_t RateMgr::ApplyReputationRate(uint32_t baseRep) const {
    return static_cast<uint32_t>(baseRep * s_rates.reputationRate);
}

} // namespace dm