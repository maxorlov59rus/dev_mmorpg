#include "game/Economy/SinkMgr.h"

namespace dm {

SinkMgr &SinkMgr::Instance() {
    static SinkMgr inst;
    return inst;
}

uint32_t SinkMgr::RepairCost(uint32_t itemLevel, uint32_t durabilityLost) const {
    // Ремонт: 10% от стоимости предмета за каждую единицу прочности
    return (itemLevel * 100 + durabilityLost * 50) / 100;
}

uint32_t SinkMgr::AuctionCommission(uint32_t salePrice) const {
    // Комиссия аукциона: 5% от цены продажи
    return salePrice * 5 / 100;
}

uint32_t SinkMgr::VendorTax(uint32_t salePrice) const {
    // Налог вендора: 10% от цены продажи
    return salePrice * 10 / 100;
}

void SinkMgr::ApplyRepair(uint64_t playerGuid, uint32_t cost) {
    // TODO: spend gold from player
    (void)playerGuid; (void)cost;
}

void SinkMgr::ApplyAuctionCommission(uint64_t playerGuid, uint32_t cost) {
    // TODO: spend gold from player
    (void)playerGuid; (void)cost;
}

} // namespace dm
