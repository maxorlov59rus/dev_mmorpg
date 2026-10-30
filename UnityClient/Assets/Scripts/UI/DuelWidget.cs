using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public class DuelWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        [SerializeField] private UIDocument uiDocument;

        private Label statusLabel;
        private Label queueCountLabel;
        private Label timerLabel;
        private Label scheduleLabel;
        private Button joinQueueButton;
        private Button leaveQueueButton;
        private ListView participantList;

        private uint playerGuid = 0;
        private bool isInQueue = false;
        private uint opponentGuid = 0;
        private uint matchId = 0;

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
            scheduleLabel = root.Q<Label>("schedule-label");
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

        public void UpdateScheduleInfo(uint8_t day, uint8_t hour, uint8_t minute)
        {
            if (scheduleLabel != null)
            {
                string dayName = day switch
                {
                    0 => "Вс",
                    1 => "Пн",
                    2 => "Вт",
                    3 => "Ср",
                    4 => "Чт",
                    5 => "Пт",
                    6 => "Сб",
                    _ => "?"
                };
                scheduleLabel.text = $\"{dayName} {hour:D2}:{minute:D2}\"";
            }
        }

        public void SetMatchId(uint id)
        {
            matchId = id;
            RefreshUI();
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
            UpdateStatus("Вы в очереди на дуэль");
            Debug.Log($"Joining duel queue as player {playerGuid}");

            // TODO: send CMSG_DUEL_JOIN
            // if (networkClient != null)
            // {
            //     networkClient.Send(new CMSG_DuelJoin
            //     {
            //         PlayerGuid = playerGuid,
            //         Level = /* player level */,
            //         SpiritLevel = /* spirit level */
            //     });
            // }
        }

        private void OnLeaveQueueClicked()
        {
            isInQueue = false;
            RefreshUI();
            UpdateStatus("Вы покинули очередь");
            Debug.Log($"Leaving duel queue");

            // TODO: send CMSG_DUEL_LEAVE
            // if (networkClient != null)
            // {
            //     networkClient.Send(new CMSG_DuelLeave
            //     {
            //         PlayerGuid = playerGuid
            //     });
            // }
        }
    }
}