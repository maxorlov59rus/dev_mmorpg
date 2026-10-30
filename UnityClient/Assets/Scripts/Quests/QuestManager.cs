// QuestManager.cs - Client-side quest manager for Unity
using UnityEngine;
using System.Collections.Generic;
using DarkMatter.Network;
#if UNITY_2022_1_OR_NEWER
using Unity.Scripting.LifecycleManagement;
#endif

namespace DM.Client
{
    /// <summary>
    /// Manages quest state on the client side.
    /// </summary>
#if UNITY_2022_1_OR_NEWER
    [NoAutoStaticsCleanup]
#endif
    public class QuestManager : MonoBehaviour
    {
        public static QuestManager Instance { get; private set; }

        [Header("Network")]
        public MMONetworkClient networkClient;

        // Active quests
        private Dictionary<int, QuestProgress> _activeQuests = new Dictionary<int, QuestProgress>();

        // Completed quests
        private HashSet<int> _completedQuests = new HashSet<int>();

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
        /// Accept a quest from an NPC.
        /// </summary>
        public void AcceptQuest(int questId)
        {
            if (_completedQuests.Contains(questId)) return;
            if (_activeQuests.ContainsKey(questId)) return;

            _activeQuests[questId] = new QuestProgress { questId = questId, progress = 0 };
            Debug.Log($"Quest {questId} accepted!");

            // TODO: Send CMSG_QUEST_ACCEPT to server
        }

        /// <summary>
        /// Update quest progress.
        /// </summary>
        public void UpdateProgress(int questId, int objectiveIndex, int count)
        {
            if (!_activeQuests.ContainsKey(questId)) return;

            var progress = _activeQuests[questId];
            progress.progress = count;
            _activeQuests[questId] = progress;

            Debug.Log($"Quest {questId} progress: {count}");

            // TODO: Send CMSG_QUEST_PROGRESS to server
        }

        /// <summary>
        /// Complete a quest.
        /// </summary>
        public void CompleteQuest(int questId)
        {
            if (!_activeQuests.ContainsKey(questId)) return;

            _activeQuests.Remove(questId);
            _completedQuests.Add(questId);
            Debug.Log($"Quest {questId} completed!");

            // TODO: Send CMSG_QUEST_COMPLETE to server
        }

        /// <summary>
        /// Check if a quest is active.
        /// </summary>
        public bool IsQuestActive(int questId)
        {
            return _activeQuests.ContainsKey(questId);
        }

        /// <summary>
        /// Check if a quest is completed.
        /// </summary>
        public bool IsQuestCompleted(int questId)
        {
            return _completedQuests.Contains(questId);
        }

        /// <summary>
        /// Get quest progress.
        /// </summary>
        public int GetQuestProgress(int questId)
        {
            if (_activeQuests.ContainsKey(questId))
                return _activeQuests[questId].progress;
            return 0;
        }
    }

    [System.Serializable]
    public struct QuestProgress
    {
        public int questId;
        public int progress;
        public int[] objectiveProgress;
    }
}