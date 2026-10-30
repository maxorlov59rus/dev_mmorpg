// SinkMgr.h — sink-механики (ремонт, комиссии, налоги)
#pragma once
#include <cstdint>

namespace dm {

class SinkMgr {
public:
    static SinkMgr &Instance();
    uint32_t RepairCost(uint32_t itemLevel, uint32_t durabilityLost) const;
    uint32_t AuctionCommission(uint32_t salePrice) const;
    uint32_t VendorTax(uint32_t salePrice) const;
    void ApplyRepair(uint64_t playerGuid, uint32_t cost);
    void ApplyAuctionCommission(uint64_t playerGuid, uint32_t cost);
};

} // namespace dm
