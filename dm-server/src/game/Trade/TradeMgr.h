// TradeMgr.h — подсистема обмена между игроками
#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <map>

namespace dm {

struct TradeState {
    uint64_t PlayerA{0};
    uint64_t PlayerB{0};
    bool AcceptedA{false};
    bool AcceptedB{false};
    uint32_t MoneyA{0};
    uint32_t MoneyB{0};
    // items stored as (itemId, count)
    std::vector<std::pair<uint32_t, uint8_t>> ItemsA;
    std::vector<std::pair<uint32_t, uint8_t>> ItemsB;
};

class TradeMgr {
public:
    static TradeMgr &Instance();
    uint64_t StartTrade(uint64_t playerA, uint64_t playerB);
    TradeState *GetTrade(uint64_t tradeId);
    void AcceptTrade(uint64_t tradeId, uint64_t playerGuid);
    void ConfirmTrade(uint64_t tradeId);
    void CancelTrade(uint64_t tradeId);
};

} // namespace dm
