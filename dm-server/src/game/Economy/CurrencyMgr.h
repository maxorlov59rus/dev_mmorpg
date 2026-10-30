// CurrencyMgr.h — экономика (валюты)
#pragma once
#include <cstdint>

namespace dm {

enum CurrencyType : uint8_t {
    CURRENCY_GOLD = 0,
    CURRENCY_LUMEN = 1,  // Орден Рассвета
    CURRENCY_UMBRA = 2,  // Круг Поглощения
    CURRENCY_SPLINTERS = 3, // Гильдия Порога
    CURRENCY_INSIGHT = 4, // Аккаунт-валюта
};

class CurrencyMgr {
public:
    static CurrencyMgr &Instance();
    void AddCurrency(uint64_t playerGuid, uint8_t currencyType, uint32_t amount, const char *reason);
    bool SpendCurrency(uint64_t playerGuid, uint8_t currencyType, uint32_t amount);
    uint32_t GetCurrency(uint64_t playerGuid, uint8_t currencyType) const;
};

} // namespace dm
