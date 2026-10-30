using UnityEngine;
using DarkMatter.Network;
using System;
using DarkMatter.UI;

namespace DM.Client
{
    public class NetworkWidgetBinder : MonoBehaviour
    {
        public MMONetworkClient networkClient;
        public GuildWidget guildWidget;
        public WeatherIndicator weatherIndicator;
        public TimeDisplay timeDisplay;
        public CharacterCreationWidget characterCreationWidget;
        public QuestLogWidget questLogWidget;
        public AuctionWidget auctionWidget;
        public CraftWidget craftWidget;
        public RelationshipWidget relationshipWidget;
        public EconomyWidget economyWidget;

        void Start()
        {
            if (networkClient == null)
            {
                networkClient = FindAnyObjectByType<MMONetworkClient>();
            }

            if (networkClient != null)
            {
                networkClient.OnPacketReceived += HandlePacket;
            }

            // Привязка виджетов к сетевому клиенту
            if (guildWidget != null)
                guildWidget.SetNetworkClient(networkClient);
            if (characterCreationWidget != null)
                characterCreationWidget.SetNetworkClient(networkClient);
            if (questLogWidget != null)
                questLogWidget.SetNetworkClient(networkClient);
            if (auctionWidget != null)
                auctionWidget.SetNetworkClient(networkClient);
            if (craftWidget != null)
                craftWidget.SetNetworkClient(networkClient);
            if (relationshipWidget != null)
                relationshipWidget.SetNetworkClient(networkClient);
            if (economyWidget != null)
                economyWidget.SetNetworkClient(networkClient);
        }

        private void HandlePacket(FMMOPacket pkt)
        {
            using (var ms = new System.IO.MemoryStream(pkt.Payload.ToArray()))
            using (var reader = new System.IO.BinaryReader(ms))
            {
                switch ((ushort)pkt.Opcode)
                {
                    case (ushort)EMMOOpcode.SMSG_CURRENCY_UPDATE:
                        var currencyPkt = CurrencyUpdatePacket.FromReader(reader);
                        economyWidget?.UpdateCurrency(currencyPkt.currencyType, currencyPkt.amount);
                        break;
                    case (ushort)EMMOOpcode.SMSG_CREATE_GUILD_RESULT:
                        var guildPkt = CreateGuildResultPacket.FromReader(reader);
                        Debug.Log($"Guild created: id={guildPkt.guildId}, result={guildPkt.result}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_AUCTION_ADD_RESULT:
                        var auctionAddPkt = AuctionAddResultPacket.FromReader(reader);
                        Debug.Log($"Auction added: id={auctionAddPkt.auctionId}, result={auctionAddPkt.result}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_AUCTION_BID_RESULT:
                        var auctionBidPkt = AuctionBidResultPacket.FromReader(reader);
                        Debug.Log($"Auction bid: id={auctionBidPkt.auctionId}, result={auctionBidPkt.result}, highest={auctionBidPkt.highestBid}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_AUCTION_BUYOUT_RESULT:
                        var auctionBuyoutPkt = AuctionBuyoutResultPacket.FromReader(reader);
                        Debug.Log($"Auction buyout: id={auctionBuyoutPkt.auctionId}, result={auctionBuyoutPkt.result}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_CRAFT_RESULT:
                        var craftPkt = CraftResultPacket.FromReader(reader);
                        Debug.Log($"Craft result: recipe={craftPkt.recipeId}, result={craftPkt.result}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_LEARN_RECIPE_RESULT:
                        var learnPkt = LearnRecipeResultPacket.FromReader(reader);
                        Debug.Log($"Learn recipe: id={learnPkt.recipeId}, result={learnPkt.result}");
                        break;
                    case (ushort)EMMOOpcode.SMSG_LEARN_PROFESSION_RESULT:
                        byte profResult = reader.ReadByte();
                        Debug.Log($"Learn profession result: {profResult}");
                        break;

                    // --- Time System ---
                    case (ushort)EMMOOpcode.SMSG_TIME_SYNC:
                        var timePkt = TimeSyncPacket.FromReader(reader);
                        timeDisplay?.UpdateFromGameTime(timePkt.hour, timePkt.minute);
                        weatherIndicator?.UpdateFromServer(timePkt);
                        break;

                    case (ushort)EMMOOpcode.SMSG_WEATHER_UPDATE:
                        var weatherPkt = WeatherUpdatePacket.FromReader(reader);
                        weatherIndicator?.UpdateFromServer(weatherPkt);
                        break;

                    // --- Resurrection System ---
                    case (ushort)EMMOOpcode.SMSG_DEATH:
                        var deathPkt = DeathPacket.FromReader(reader);
                        var resClient = FindAnyObjectByType<ResurrectionClient>();
                        if (resClient != null)
                            resClient.OnPlayerDeath(new Vector3(deathPkt.corpseX, deathPkt.corpseY, deathPkt.corpseZ));
                        break;

                    case (ushort)EMMOOpcode.SMSG_RESURRECT:
                        var resPkt = ResurrectPacket.FromReader(reader);
                        var resClient2 = FindAnyObjectByType<ResurrectionClient>();
                        if (resClient2 != null)
                        {
                            if (resPkt.type == 1) // ally res
                                resClient2.OnResurrectByAlly();
                            else if (resPkt.type == 2) // self res
                                resClient2.OnResurrectByItem();
                            else if (resPkt.type == 3) // spirit healer
                                resClient2.OnResurrectAtSpiritHealer(new Vector3(resPkt.x, resPkt.y, resPkt.z));
                        }
                        break;

                    // --- Character Creation ---
                    case (ushort)EMMOOpcode.SMSG_CHAR_CREATE_SUCCESS:
                        characterCreationWidget?.HandleCreateSuccess();
                        break;

                    case (ushort)EMMOOpcode.SMSG_CHAR_CREATE_ERROR:
                        byte errorCode = reader.ReadByte();
                        characterCreationWidget?.HandleCreateError(errorCode);
                        break;
                }
            }
        }

        void OnDestroy()
        {
            if (networkClient != null)
                networkClient.OnPacketReceived -= HandlePacket;
        }
    }
}
