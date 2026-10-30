using UnityEngine;
using DarkMatter.Network;
using System;

namespace DM.Client
{
    public class GuildWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        public void CreateGuild(string name)
        {
            Debug.Log($"Creating guild: {name}");
            // TODO: send CMSG_CREATE_GUILD
        }

        public void AddMember(uint guildId, uint playerGuid, byte rank)
        {
            Debug.Log($"Adding member {playerGuid} to guild {guildId} with rank {rank}");
            // TODO: send CMSG_GUILD_ADD_MEMBER
        }

        public void RemoveMember(uint guildId, uint playerGuid)
        {
            Debug.Log($"Removing member {playerGuid} from guild {guildId}");
            // TODO: send CMSG_GUILD_REMOVE_MEMBER
        }
    }
}
