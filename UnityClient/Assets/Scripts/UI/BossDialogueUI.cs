// BossDialogueUI.cs — UI диалогов боссов (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    public class BossDialogueUI : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _dialoguePanel;
        private Label _bossNameLabel;
        private Label _dialogueText;
        private Button _closeButton;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _dialoguePanel = root.Q<VisualElement>("BossDialoguePanel");
                _bossNameLabel = root.Q<Label>("BossName");
                _dialogueText = root.Q<Label>("BossDialogueText");
                _closeButton = root.Q<Button>("CloseButton");

                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                HideDialogue();
            }
        }

        void OnDisable()
        {
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        public void ShowBossDialogue(string bossName, string dialogueText)
        {
            if (_bossNameLabel != null)
                _bossNameLabel.text = bossName;
            if (_dialogueText != null)
                _dialogueText.text = dialogueText;

            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.Flex;

            UnityEngine.Cursor.lockState = CursorLockMode.None;
            UnityEngine.Cursor.visible = true;
        }

        public void ShowBossDeathDialogue(string bossName, string dialogueText)
        {
            ShowBossDialogue(bossName, dialogueText);
        }

        void HideDialogue()
        {
            if (_dialoguePanel != null)
                _dialoguePanel.style.display = DisplayStyle.None;

            UnityEngine.Cursor.lockState = CursorLockMode.Locked;
            UnityEngine.Cursor.visible = false;
        }

        void OnCloseClicked()
        {
            HideDialogue();
        }

        void OnDestroy()
        {
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}
