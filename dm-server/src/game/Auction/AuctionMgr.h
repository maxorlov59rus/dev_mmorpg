// AuctionMgr.h — аукцион
#pragma once
#include <cstdint>

namespace dm {

class AuctionMgr {
public:
    static AuctionMgr &Instance();
    uint32_t AddAuction(uint32_t itemId, uint64_t ownerGuid, uint32_t buyoutPrice, uint32_t startPrice);
    void RemoveAuction(uint32_t auctionId);
    bool Bid(uint32_t auctionId, uint64_t bidderGuid, uint32_t amount);
    bool Buyout(uint32_t auctionId, uint64_t buyerGuid);
    uint32_t GetHighestBid(uint32_t auctionId) const;
    void LoadFromDB();
    void SaveToDB(uint32_t auctionId);
};

} // namespace dm
