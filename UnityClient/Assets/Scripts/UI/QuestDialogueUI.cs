// QuestDialogueUI.cs — UI диалогов квестов (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;

namespace DarkMatter.UI
{
    public class QuestDialogueUI : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _dialoguePanel;
        private Label _npcNameLabel;
        private Label _npcTitleLabel;
        private Label _dialogueText;
        private Button _acceptButton;
        private Button _declineButton;
        private Button _completeButton;
        private Button _closeButton;
        private VisualElement _rewardPanel;
        private Label _rewardText;

        private int _currentQuestId = 0;
        private int _currentNpcId = 0;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _dialoguePanel = root.Q<VisualElement>("DialoguePanel");
                _npcNameLabel = root.Q<Label>("NpcName");
                _npcTitleLabel = root.Q<Label>("NpcTitle");
                _dialogueText = root.Q<Label>("DialogueText");
                _acceptButton = root.Q<Button>("AcceptButton");
                _declineButton = root.Q<Button>("DeclineButton");
                _completeButton = root.Q<Button>("CompleteButton");
                _closeButton = root.Q<Button>("CloseButton");
                _rewardPanel = root.Q<VisualElement>("RewardPanel");
                _rewardText = root.Q<Label>("RewardText");

                if (_acceptButton != null)
                    _acceptButton.clicked += OnAcceptClicked;
                if (_declineButton != null)
                    _declineButton.clicked += OnDeclineClicked;
                if (_completeButton != null)
                    _completeButton.clicked += OnCompleteClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                HideDialogue();
            }
        }

        void OnDisable()
        {
            if (_acceptButton != null)
                _acceptButton.clicked -= OnAcceptClicked;
            if (_declineButton != null)
                _declineButton.clicked -= OnDeclineClicked;
            if (_completeButton != null)
                _completeButton.clicked -= OnCompleteClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        public void ShowDialogue(int questId, int npcId, string npcName, string npcTitle, string dialogueText, bool canAccept, bool canComplete)
        {
            _currentQuestId = questId;
            _currentNpcId = npcId;

            if (_npcNameLabel != null)
                _npcNameLabel.text = npcName;
            if (_npcTitleLabel != null)
                _npcTitleLabel.text = npcTitle;
            if (_dialogueText != null)
                _dialogueText.text = dialogueText;

            if (_acceptButton != null)
                _acceptButton.style.display = canAccept ? DisplayStyle.Flex : DisplayStyle.None;
            if (_declineButton != null)
                _declineButton.style.display = canAccept ? DisplayStyle.Flex : DisplayStyle.None;
            if (_completeButton != null)
                _completeButton.style.display = canComplete ? DisplayStyle.Flex : DisplayStyle.None;

            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.Flex;

            // Lock cursor
            UnityEngine.Cursor.lockState = CursorLockMode.None;
            UnityEngine.Cursor.visible = true;
        }

        public void ShowReward(string rewardText)
        {
            if (_rewardPanel != null)
                _rewardPanel.style.display = DisplayStyle.Flex;
            if (_rewardText != null)
                _rewardText.text = rewardText;
        }

        void HideDialogue()
        {
            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.None;
            if (_rewardPanel != null)
                _rewardPanel.style.display = DisplayStyle.None;

            UnityEngine.Cursor.lockState = CursorLockMode.Locked;
            UnityEngine.Cursor.visible = false;
        }

        void OnAcceptClicked()
        {
            Debug.Log($"Accepted quest {_currentQuestId} from NPC {_currentNpcId}");
            // TODO: Send CMSG_QUEST_ACCEPT to server
            HideDialogue();
        }

        void OnDeclineClicked()
        {
            Debug.Log($"Declined quest {_currentQuestId}");
            HideDialogue();
        }

        void OnCompleteClicked()
        {
            Debug.Log($"Completing quest {_currentQuestId}");
            // TODO: Send CMSG_QUEST_COMPLETE to server
            HideDialogue();
        }

        void OnCloseClicked()
        {
            HideDialogue();
        }

        void OnDestroy()
        {
            if (_acceptButton != null)
                _acceptButton.clicked -= OnAcceptClicked;
            if (_declineButton != null)
                _declineButton.clicked -= OnDeclineClicked;
            if (_completeButton != null)
                _completeButton.clicked -= OnCompleteClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}
