// QuestRewardManager.cs — Управление наградами за квесты
using UnityEngine;
using DarkMatter.Network;
using System.Collections.Generic;
#if UNITY_2022_1_OR_NEWER
using Unity.Scripting.LifecycleManagement;
#endif

namespace DM.Client
{
#if UNITY_2022_1_OR_NEWER
    [NoAutoStaticsCleanup]
#endif
    public class QuestRewardManager : MonoBehaviour
    {
        public static QuestRewardManager Instance { get; private set; }

        [Header("Network")]
        public MMONetworkClient networkClient;

        void Awake()
        {
            if (Instance == null)
                Instance = this;
            else
                Destroy(gameObject);
        }

        void Start()
        {
            if (networkClient == null)
                networkClient = FindAnyObjectByType<MMONetworkClient>();
        }

        /// <summary>
        /// Grant quest rewards to player.
        /// </summary>
        public void GrantRewards(int questId, int rewardMoney, int rewardRepFaction, int rewardRepAmount, List<int> rewardItems)
        {
            Debug.Log($"Quest {questId} rewards: {rewardMoney} gold, Faction {rewardRepFaction} +{rewardRepAmount} rep");

            if (rewardItems != null)
            {
                foreach (int itemId in rewardItems)
                {
                    Debug.Log($"  Item reward: {itemId}");
                    // TODO: Add item to player inventory
                }
            }

            // TODO: Send quest complete packet to server
            // TODO: Update player gold, reputation, inventory
        }

        /// <summary>
        /// Show reward popup.
        /// </summary>
        public void ShowRewardPopup(int questId, string questName, int rewardMoney, int rewardRepAmount, List<string> rewardItemNames)
        {
            // TODO: Show UI popup with rewards
            Debug.Log($"Reward popup for quest {questId}: {questName}");
            Debug.Log($"  Gold: {rewardMoney}");
            Debug.Log($"  Reputation: +{rewardRepAmount}");
            if (rewardItemNames != null)
            {
                foreach (string itemName in rewardItemNames)
                {
                    Debug.Log($"  Item: {itemName}");
                }
            }
        }
    }
}