// QuestNPC.cs - NPC controller for quest givers in Unity
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    /// <summary>
    /// Controls quest NPC behavior, dialogue, and quest giving.
    /// </summary>
    public class QuestNPC : MonoBehaviour
    {
        [Header("NPC Info")]
        public string npcName = "Quest Giver";
        public string npcTitle = "Quest Giver";
        public int npcId = 70000;

        [Header("Quests")]
        public int[] questIds = new int[0];

        [Header("Interaction")]
        public float interactionRange = 3f;
        public GameObject interactionPrompt;

        private Transform _playerTransform;
        private bool _playerInRange = false;
        private bool _isDialogueOpen = false;

        [Header("UI")]
        private VisualElement _dialoguePanel;
        private Label _npcNameLabel;
        private Label _npcTitleLabel;
        private Label _dialogueText;
        private Button _acceptButton;
        private Button _declineButton;
        private Button _closeButton;
        private UIDocument _uiDocument;

        void Start()
        {
            var player = GameObject.FindWithTag("Player");
            if (player != null)
                _playerTransform = player.transform;

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
                _closeButton = root.Q<Button>("CloseButton");

                if (_acceptButton != null)
                    _acceptButton.clicked += OnAcceptClicked;
                if (_declineButton != null)
                    _declineButton.clicked += OnDeclineClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                if (_dialoguePanel != null)
                    _dialoguePanel.style.display = DisplayStyle.None;
            }

            if (interactionPrompt != null)
                interactionPrompt.SetActive(false);
        }

        void Update()
        {
            if (_playerTransform == null) return;

            float distance = Vector3.Distance(transform.position, _playerTransform.position);
            bool inRange = distance <= interactionRange;

            if (inRange != _playerInRange)
            {
                _playerInRange = inRange;
                if (interactionPrompt != null)
                    interactionPrompt.SetActive(inRange);
            }

            // Face player
            if (inRange)
            {
                Vector3 direction = (_playerTransform.position - transform.position).normalized;
                direction.y = 0;
                if (direction != Vector3.zero)
                    transform.rotation = Quaternion.LookRotation(direction);
            }

            // Open dialogue on interact
            if (_playerInRange && Input.GetKeyDown(KeyCode.E) && !_isDialogueOpen)
            {
                OpenDialogue();
            }
        }

        void OpenDialogue()
        {
            _isDialogueOpen = true;
            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.Flex;

            if (_npcNameLabel != null)
                _npcNameLabel.text = npcName;
            if (_npcTitleLabel != null)
                _npcTitleLabel.text = npcTitle;

            // Show first quest dialogue
            if (_dialogueText != null && questIds.Length > 0)
            {
                _dialogueText.text = $"Greetings, adventurer! I have {questIds.Length} quest(s) for you.";
            }

            // Lock cursor
            UnityEngine.Cursor.lockState = CursorLockMode.None;
            UnityEngine.Cursor.visible = true;
        }

        void CloseDialogue()
        {
            _isDialogueOpen = false;
            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.None;

            UnityEngine.Cursor.lockState = CursorLockMode.Locked;
            UnityEngine.Cursor.visible = false;
        }

        void OnAcceptClicked()
        {
            Debug.Log($"Accepted quest from {npcName}");
            // TODO: Send quest accept packet to server
            CloseDialogue();
        }

        void OnDeclineClicked()
        {
            Debug.Log($"Declined quest from {npcName}");
            CloseDialogue();
        }

        void OnCloseClicked()
        {
            CloseDialogue();
        }

        void OnDestroy()
        {
            if (_acceptButton != null)
                _acceptButton.clicked -= OnAcceptClicked;
            if (_declineButton != null)
                _declineButton.clicked -= OnDeclineClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}