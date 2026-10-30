using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public class SpiritBattleWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        [SerializeField] private UIDocument uiDocument;

        private Label statusLabel;
        private Label queueCountLabel;
        private Label timerLabel;
        private Button joinQueueButton;
        private Button leaveQueueButton;
        private ListView participantList;

        private uint playerGuid = 0;
        private bool isInQueue = false;

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            statusLabel = root.Q<Label>("status-label");
            queueCountLabel = root.Q<Label>("queue-count");
            timerLabel = root.Q<Label>("timer-label");
            joinQueueButton = root.Q<Button>("join-queue-button");
            leaveQueueButton = root.Q<Button>("leave-queue-button");
            participantList = root.Q<ListView>("participant-list");

            if (joinQueueButton != null) joinQueueButton.clicked += OnJoinQueueClicked;
            if (leaveQueueButton != null) leaveQueueButton.clicked += OnLeaveQueueClicked;

            RefreshUI();
        }

        private void OnDisable()
        {
            if (joinQueueButton != null) joinQueueButton.clicked -= OnJoinQueueClicked;
            if (leaveQueueButton != null) leaveQueueButton.clicked -= OnLeaveQueueClicked;
        }

        public void SetPlayerInfo(uint guid)
        {
            playerGuid = guid;
            RefreshUI();
        }

        public void UpdateQueueCount(int count)
        {
            if (queueCountLabel != null)
                queueCountLabel.text = $"В очереди: {count}";
        }

        public void UpdateStatus(string status)
        {
            if (statusLabel != null)
                statusLabel.text = status;
        }

        public void UpdateTimer(int seconds)
        {
            if (timerLabel != null)
                timerLabel.text = $"Время: {seconds / 60}:{seconds % 60:D2}";
        }

        private void RefreshUI()
        {
            if (joinQueueButton != null)
                joinQueueButton.style.display = isInQueue ? DisplayStyle.None : DisplayStyle.Flex;
            if (leaveQueueButton != null)
                leaveQueueButton.style.display = isInQueue ? DisplayStyle.Flex : DisplayStyle.None;
        }

        private void OnJoinQueueClicked()
        {
            isInQueue = true;
            RefreshUI();
            UpdateStatus("Вы в очереди на Битву Духов");
            Debug.Log("Joining Spirit Battle queue");
            // TODO: send CMSG_SPIRIT_BATTLE_JOIN
        }

        private void OnLeaveQueueClicked()
        {
            isInQueue = false;
            RefreshUI();
            UpdateStatus("Вы покинули очередь");
            Debug.Log("Leaving Spirit Battle queue");
            // TODO: send CMSG_SPIRIT_BATTLE_LEAVE
        }
    }
}
