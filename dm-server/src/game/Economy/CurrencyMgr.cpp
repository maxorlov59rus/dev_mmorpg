#include "game/Economy/CurrencyMgr.h"
#include <unordered_map>

namespace dm {

CurrencyMgr &CurrencyMgr::Instance() {
    static CurrencyMgr inst;
    return inst;
}

static std::unordered_map<uint64_t, std::unordered_map<uint8_t, uint32_t>> s_currencies;

void CurrencyMgr::AddCurrency(uint64_t playerGuid, uint8_t currencyType, uint32_t amount, const char *reason) {
    s_currencies[playerGuid][currencyType] += amount;
    // TODO: send SMSG_CURRENCY_UPDATE
}

bool CurrencyMgr::SpendCurrency(uint64_t playerGuid, uint8_t currencyType, uint32_t amount) {
    auto &cur = s_currencies[playerGuid][currencyType];
    if (cur < amount) return false;
    cur -= amount;
    return true;
}

uint32_t CurrencyMgr::GetCurrency(uint64_t playerGuid, uint8_t currencyType) const {
    auto it = s_currencies.find(playerGuid);
    if (it == s_currencies.end()) return 0;
    auto it2 = it->second.find(currencyType);
    if (it2 == it->second.end()) return 0;
    return it2->second;
}

} // namespace dm
