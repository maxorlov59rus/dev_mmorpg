// VendorMgr.h
#pragma once
#include <cstdint>
#include <vector>

namespace dm {

struct VendorItem {
    uint32_t ItemId;
    uint32_t Price;
    uint8_t StackCount;
};

class VendorMgr {
public:
    static VendorMgr &Instance();
    void LoadVendor(uint32_t npcGuid, const std::vector<VendorItem> &items);
    const std::vector<VendorItem> *GetVendorItems(uint32_t npcGuid) const;
    bool CanBuy(uint32_t npcGuid, uint32_t itemId, uint8_t count, uint32_t playerMoney,
                uint8_t freeSlots) const;
    bool BuyItem(uint32_t npcGuid, uint32_t itemId, uint8_t count, uint32_t &playerMoney,
                 uint8_t &freeSlots);
    bool CanSell(uint64_t itemGuid, uint8_t count, uint32_t &outPrice) const;
    bool SellItem(uint64_t itemGuid, uint8_t count, uint32_t &playerMoney);
};

} // namespace dm
