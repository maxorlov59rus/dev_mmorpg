using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class RelationshipWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        [SerializeField] private UIDocument uiDocument;

        private ListView friendList;
        private ListView relationshipList;
        private TextField friendNameInput;
        private Button addFriendButton;
        private Button removeFriendButton;
        private Button blockButton;
        private Button proposeButton;
        private Button acceptButton;
        private Button declineButton;
        private Button dissolveButton;
        private Label statusLabel;
        private Label partnerLabel;
        private Label relationshipTypeLabel;

        private uint playerGuid = 0;
        private uint playerLevel = 1;
        private uint selectedFriendGuid = 0;
        private uint selectedPartnerGuid = 0;

        private List<FriendEntryData> friends = new List<FriendEntryData>();
        private List<RelationshipEntryData> relationships = new List<RelationshipEntryData>();

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            friendList = root.Q<ListView>("friend-list");
            relationshipList = root.Q<ListView>("relationship-list");
            friendNameInput = root.Q<TextField>("friend-name-input");
            addFriendButton = root.Q<Button>("add-friend-button");
            removeFriendButton = root.Q<Button>("remove-friend-button");
            blockButton = root.Q<Button>("block-button");
            proposeButton = root.Q<Button>("propose-button");
            acceptButton = root.Q<Button>("accept-button");
            declineButton = root.Q<Button>("decline-button");
            dissolveButton = root.Q<Button>("dissolve-button");
            statusLabel = root.Q<Label>("status-label");
            partnerLabel = root.Q<Label>("partner-label");
            relationshipTypeLabel = root.Q<Label>("relationship-type-label");

            if (addFriendButton != null) addFriendButton.clicked += OnAddFriendClicked;
            if (removeFriendButton != null) removeFriendButton.clicked += OnRemoveFriendClicked;
            if (blockButton != null) blockButton.clicked += OnBlockClicked;
            if (proposeButton != null) proposeButton.clicked += OnProposeClicked;
            if (acceptButton != null) acceptButton.clicked += OnAcceptClicked;
            if (declineButton != null) declineButton.clicked += OnDeclineClicked;
            if (dissolveButton != null) dissolveButton.clicked += OnDissolveClicked;

            RefreshUI();
        }

        private void OnDisable()
        {
            if (addFriendButton != null) addFriendButton.clicked -= OnAddFriendClicked;
            if (removeFriendButton != null) removeFriendButton.clicked -= OnRemoveFriendClicked;
            if (blockButton != null) blockButton.clicked -= OnBlockClicked;
            if (proposeButton != null) proposeButton.clicked -= OnProposeClicked;
            if (acceptButton != null) acceptButton.clicked -= OnAcceptClicked;
            if (declineButton != null) declineButton.clicked -= OnDeclineClicked;
            if (dissolveButton != null) dissolveButton.clicked -= OnDissolveClicked;
        }

        public void SetPlayerInfo(uint guid, uint level)
        {
            playerGuid = guid;
            playerLevel = level;
            RefreshUI();
        }

        public void AddFriend(FriendEntryData friend)
        {
            friends.Add(friend);
            RefreshFriendList();
        }

        public void RemoveFriend(uint friendGuid)
        {
            friends.RemoveAll(f => f.FriendGuid == friendGuid);
            RefreshFriendList();
        }

        public void AddRelationship(RelationshipEntryData rel)
        {
            relationships.Add(rel);
            RefreshRelationshipList();
        }
        private void RefreshUI()
        {
            RefreshFriendList();
            RefreshRelationshipList();
            UpdateStatus();
        }

        private void RefreshFriendList()
        {
            if (friendList == null) return;

            var items = friends.Select(f => $"{f.FriendName} ({f.Status})").ToList();
            friendList.itemsSource = items;
            friendList.makeItem = () => new Label();
            friendList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
            friendList.selectionChanged += OnFriendSelected;
        }

        private void OnFriendSelected(IEnumerable<object> selectedItems)
        {
            var index = friendList.selectedIndex;
            if (index < 0 || index >= friends.Count) return;
            selectedFriendGuid = friends[index].FriendGuid;
        }

        private void RefreshRelationshipList()
        {
            if (relationshipList == null) return;

            var items = relationships.Select(r => $"{r.PartnerName} - {RelationshipData.RelationshipTypeNamesRu[(int)r.Type]}").ToList();
            relationshipList.itemsSource = items;
            relationshipList.makeItem = () => new Label();
            relationshipList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
            relationshipList.selectionChanged += OnRelationshipSelected;
        }

        private void OnRelationshipSelected(IEnumerable<object> selectedItems)
        {
            var index = relationshipList.selectedIndex;
            if (index < 0 || index >= relationships.Count) return;
            selectedPartnerGuid = relationships[index].Player2Guid;
            if (partnerLabel != null) partnerLabel.text = relationships[index].PartnerName;
            if (relationshipTypeLabel != null) relationshipTypeLabel.text = RelationshipData.RelationshipTypeNamesRu[(int)relationships[index].Type];
        }

        private void UpdateStatus()
        {
            if (statusLabel != null)
            {
                var activeRel = relationships.FirstOrDefault(r => r.Status == RelationshipStatus.Active);
                if (activeRel != null)
                    statusLabel.text = $"В отношениях с {activeRel.PartnerName}";
                else
                    statusLabel.text = "Не в отношениях";
            }
        }

        private void OnAddFriendClicked()
        {
            if (friendNameInput == null || string.IsNullOrEmpty(friendNameInput.value)) return;
            string targetName = friendNameInput.value;
            Debug.Log($"Sending friend request to {targetName}");
        }

        private void OnRemoveFriendClicked()
        {
            if (selectedFriendGuid == 0) return;
            RemoveFriend(selectedFriendGuid);
            Debug.Log($"Removing friend {selectedFriendGuid}");
        }

        private void OnBlockClicked()
        {
            if (selectedFriendGuid == 0) return;
            Debug.Log($"Blocking player {selectedFriendGuid}");
        }

        private void OnProposeClicked()
        {
            if (selectedFriendGuid == 0) return;
            Debug.Log($"Proposing relationship to {selectedFriendGuid}");
        }

        private void OnAcceptClicked()
        {
            if (selectedPartnerGuid == 0) return;
            Debug.Log($"Accepting relationship with {selectedPartnerGuid}");
        }

        private void OnDeclineClicked()
        {
            if (selectedPartnerGuid == 0) return;
            Debug.Log($"Declining relationship with {selectedPartnerGuid}");
        }

        private void OnDissolveClicked()
        {
            if (selectedPartnerGuid == 0) return;
            Debug.Log($"Dissolving relationship with {selectedPartnerGuid}");
        }
    }
}
