#include "game/Auction/AuctionMgr.h"
#include <unordered_map>

namespace dm {

struct Auction {
    uint32_t itemId;
    uint64_t ownerGuid;
    uint32_t buyoutPrice;
    uint32_t startPrice;
    uint64_t highestBidder;
    uint32_t highestBid;
    bool active;
};

static uint32_t s_nextAuctionId = 1;
static std::unordered_map<uint32_t, Auction> s_auctions;

AuctionMgr &AuctionMgr::Instance() {
    static AuctionMgr inst;
    return inst;
}

uint32_t AuctionMgr::AddAuction(uint32_t itemId, uint64_t ownerGuid, uint32_t buyoutPrice, uint32_t startPrice) {
    uint32_t auctionId = s_nextAuctionId++;
    s_auctions[auctionId] = {itemId, ownerGuid, buyoutPrice, startPrice, 0, startPrice, true};
    return auctionId;
}

void AuctionMgr::RemoveAuction(uint32_t auctionId) {
    auto it = s_auctions.find(auctionId);
    if (it != s_auctions.end()) {
        it->second.active = false;
    }
}

bool AuctionMgr::Bid(uint32_t auctionId, uint64_t bidderGuid, uint32_t amount) {
    auto it = s_auctions.find(auctionId);
    if (it == s_auctions.end() || !it->second.active) return false;
    if (amount <= it->second.highestBid) return false;
    it->second.highestBidder = bidderGuid;
    it->second.highestBid = amount;
    return true;
}

bool AuctionMgr::Buyout(uint32_t auctionId, uint64_t buyerGuid) {
    auto it = s_auctions.find(auctionId);
    if (it == s_auctions.end() || !it->second.active) return false;
    (void)buyerGuid;
    it->second.active = false;
    return true;
}

uint32_t AuctionMgr::GetHighestBid(uint32_t auctionId) const {
    auto it = s_auctions.find(auctionId);
    if (it == s_auctions.end()) return 0;
    return it->second.highestBid;
}

void AuctionMgr::LoadFromDB() {
    // TODO: SELECT * FROM auctions WHERE active = 1
}

void AuctionMgr::SaveToDB(uint32_t auctionId) {
    // TODO: INSERT/UPDATE auction in DB
    (void)auctionId;
}

} // namespace dm
