// CharacterCreationWidget.cs
// Handles character creation with template selection and server communication using UI Toolkit.
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;
using DarkMatter.Network;

namespace DM.Client
{
    /// <summary>
    /// Handles character creation with template selection, name input, and server communication.
    /// </summary>
    public class CharacterCreationWidget : MonoBehaviour
    {
        [Header("Network")]
        public MMONetworkClient networkClient;

        [Header("Templates")]
        public List<CharacterTemplate> characterTemplates = new List<CharacterTemplate>();

        private UIDocument _uiDocument;
        private DropdownField _templateDropdown;
        private DropdownField _genderDropdown;
        private TextField _nameInputField;
        private Button _createButton;
        private Button _cancelButton;
        private Label _statusLabel;

        private int _selectedTemplateIndex = 0;
        private int _selectedGender = 0;
        private bool _isCreating = false;
        private const int MAX_NAME_LENGTH = 24;

        [System.Serializable]
        public class CharacterTemplate
        {
            public string displayName;
            public byte classId;
            public byte factionId;
            public byte gender; // 0 = male, 1 = female
        }

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _templateDropdown = root.Q<DropdownField>("TemplateDropdown");
                _genderDropdown = root.Q<DropdownField>("GenderDropdown");
                _nameInputField = root.Q<TextField>("NameInputField");
                _createButton = root.Q<Button>("CreateButton");
                _cancelButton = root.Q<Button>("CancelButton");
                if (_templateDropdown != null)
                {
                    _templateDropdown.choices.Clear();
                    foreach (var template in characterTemplates)
                    {
                        _templateDropdown.choices.Add(template.displayName);
                    }
                    _templateDropdown.index = 0;
                    _templateDropdown.RegisterValueChangedCallback(OnTemplateChanged);
                }


                _statusLabel = root.Q<Label>("StatusLabel");

                if (_genderDropdown != null)
                {
                    _genderDropdown.choices.Clear();
                    _genderDropdown.choices.Add("Мужской");
                    _genderDropdown.choices.Add("Женский");
                    _genderDropdown.index = 0;
                    _genderDropdown.RegisterValueChangedCallback(OnGenderChanged);
                }

                if (_nameInputField != null)
                    _nameInputField.RegisterValueChangedCallback(OnNameChanged);

                if (_createButton != null)
                {
                    _createButton.SetEnabled(false);
                    _createButton.clicked += OnCreateClicked;
                }

                if (_cancelButton != null)
                    _cancelButton.clicked += OnCancelClicked;
            }

            if (networkClient == null)
                networkClient = FindAnyObjectByType<MMONetworkClient>();
        }

        void OnDisable()
        {
            if (_templateDropdown != null)
                _templateDropdown.UnregisterValueChangedCallback(OnTemplateChanged);

            if (_genderDropdown != null)
                _genderDropdown.UnregisterValueChangedCallback(OnGenderChanged);

            if (_nameInputField != null)
                _nameInputField.UnregisterValueChangedCallback(OnNameChanged);

            if (_createButton != null)
                _createButton.clicked -= OnCreateClicked;

            if (_cancelButton != null)
                _cancelButton.clicked -= OnCancelClicked;
        }
        void OnNameChanged(ChangeEvent<string> evt)
        {
            string text = evt.newValue;

            if (string.IsNullOrEmpty(text))
            {
                SetStatus("Enter a character name");
                if (_createButton != null)
                    _createButton.SetEnabled(false);
            }
            else if (text.Length > MAX_NAME_LENGTH)
            {
                SetStatus($"Name too long (max {MAX_NAME_LENGTH} chars)");
                if (_createButton != null)
                    _createButton.SetEnabled(false);
            }
            else
            {
                if (_createButton != null)
                    _createButton.SetEnabled(true);
                SetStatus($"Ready to create: {text}");
            }
        }

        void OnGenderChanged(ChangeEvent<string> evt)
        {
            _selectedGender = _genderDropdown.index;
        }

        void OnTemplateChanged(ChangeEvent<string> evt)
        {
            _selectedTemplateIndex = _templateDropdown.index;

            if (_selectedTemplateIndex >= 0 && _selectedTemplateIndex < characterTemplates.Count)
            {
                var template = characterTemplates[_selectedTemplateIndex];
                SetStatus($"Template: {template.displayName}");
            }
        }

        void OnCreateClicked()
        {
            if (_isCreating) return;
            if (string.IsNullOrEmpty(_nameInputField != null ? _nameInputField.value : null))
            {
                SetStatus("Name cannot be empty");
                return;
            }

            _isCreating = true;
            SetStatus("Creating character...");
            if (_createButton != null)
                _createButton.SetEnabled(false);

            SendCreateCharacterRequest();
        }

        void OnCancelClicked()
        {
            gameObject.SetActive(false);
        }

        void SetStatus(string message)
        {
            if (_statusLabel != null)
                _statusLabel.text = message;
        }

        void SendCreateCharacterRequest()
        {
            if (networkClient == null) return;

            var template = characterTemplates[_selectedTemplateIndex];
            Debug.Log($"Sending CMSG_CHAR_CREATE: name={_nameInputField.value}, template={template.displayName}, class={template.classId}, faction={template.factionId}, gender={_selectedGender}");

            Invoke("SimulateServerResponse", 2f);
        }

        public void HandleCreateSuccess()
        {
            _isCreating = false;
            if (_createButton != null)
                _createButton.SetEnabled(true);
            SetStatus($"Character created! Select \"{_nameInputField.value}\" in the character list.");
            Invoke("HideWindow", 3f);
        }

        public void HandleCreateError(byte errorCode)
        {
            _isCreating = false;
            if (_createButton != null)
                _createButton.SetEnabled(true);

            string errorMsg = GetErrorMessage(errorCode);
            SetStatus($"Create failed: {errorMsg}");
            Debug.LogWarning($"Character creation failed: {errorMsg}");
        }

        string GetErrorMessage(byte errorCode)
        {
            switch (errorCode)
            {
                case 0: return "Name is already taken";
                case 1: return "Invalid name";
                case 2: return "Server is full";
                case 3: return "Character limit reached";
                default: return "Unknown error";
            }
        }

        void HideWindow()
        {
            gameObject.SetActive(false);
        }

        void SimulateServerResponse()
        {
            bool success = UnityEngine.Random.value > 0.25f;

            if (success)
            {
                HandleCreateSuccess();
            }
            else
            {
                HandleCreateError(0);
            }
        }

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }
    }
}
