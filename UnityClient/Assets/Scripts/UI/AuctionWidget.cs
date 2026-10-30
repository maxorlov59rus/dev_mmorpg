using UnityEngine;
using DarkMatter.Network;
using System;

namespace DM.Client
{
    public class AuctionWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        public void AddAuction(uint itemId, ulong ownerGuid, uint buyoutPrice, uint startPrice)
        {
            Debug.Log($"Adding auction: item={itemId}, owner={ownerGuid}, buyout={buyoutPrice}, start={startPrice}");
            // TODO: send CMSG_AUCTION_ADD
        }

        public void Bid(uint auctionId, ulong bidderGuid, uint amount)
        {
            Debug.Log($"Bidding on auction {auctionId}: bidder={bidderGuid}, amount={amount}");
            // TODO: send CMSG_AUCTION_BID
        }

        public void Buyout(uint auctionId, ulong buyerGuid)
        {
            Debug.Log($"Buying out auction {auctionId}: buyer={buyerGuid}");
            // TODO: send CMSG_AUCTION_BUYOUT
        }
    }
}
