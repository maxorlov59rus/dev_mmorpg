// QuestLogWidget.cs — UI журнала квестов (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;
using DarkMatter.Network;
using DarkMatter.UI;

namespace DarkMatter.UI
{
    public class QuestLogWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;

        private ListView _questList;
        private Label _questTitle;
        private Label _questDescription;
        private Label _objectiveText;
        private Button _trackButton;
        private Button _abandonButton;
        private Button _specChoiceButton;

        private int _selectedQuestId = 0;
        private Dictionary<int, QuestData> _quests = new Dictionary<int, QuestData>();

        [System.Serializable]
        public class QuestData
        {
            public int questId;
            public string title;
            public string description;
            public string objectiveText;
            public int minLevel;
            public int maxLevel;
            public string zoneName;
            public string category;
            public int nextQuestId;
            public bool isSpecChoice;
            public bool isLfgGate;
            public int rewardMoney;
            public int progress;
        }

        void OnEnable()
        {
            root = GetComponent<UIDocument>()?.rootVisualElement;
            if (root == null) return;

            _questList = root.Q<ListView>("QuestList");
            _questTitle = root.Q<Label>("QuestTitle");
            _questDescription = root.Q<Label>("QuestDescription");
            _objectiveText = root.Q<Label>("ObjectiveText");
            _trackButton = root.Q<Button>("TrackButton");
            _abandonButton = root.Q<Button>("AbandonButton");
            _specChoiceButton = root.Q<Button>("SpecChoiceButton");

            if (_trackButton != null)
                _trackButton.clicked += OnTrackClicked;
            if (_abandonButton != null)
                _abandonButton.clicked += OnAbandonClicked;
            if (_specChoiceButton != null)
            {
                _specChoiceButton.clicked += OnSpecChoiceClicked;
                _specChoiceButton.style.display = DisplayStyle.None;
            }

            if (_questList != null)
            {
                _questList.makeItem = () => new Label();
                _questList.bindItem = (element, index) =>
                {
                    var label = element as Label;
                    var keys = new List<int>(_quests.Keys);
                    if (index < keys.Count)
                    {
                        int qid = keys[index];
                        label.text = _quests[qid].title;
                    }
                };
                _questList.selectionChanged += OnQuestSelected;
            }

            LoadQuests();
        }
        void OnDisable()
        {
            if (_trackButton != null)
                _trackButton.clicked -= OnTrackClicked;
            if (_abandonButton != null)
                _abandonButton.clicked -= OnAbandonClicked;
            if (_specChoiceButton != null)
                _specChoiceButton.clicked -= OnSpecChoiceClicked;
        }

        void LoadQuests()
        {
            AddQuest(new QuestData
            {
                questId = 1001,
                title = "Первые испытания",
                description = "Слизни заполонили тропу у ручья. Командор Велан просит очистить её.",
                objectiveText = "Убить слизней: 0 / 5",
                minLevel = 6,
                maxLevel = 8,
                zoneName = "Лес Испытаний",
                category = "PvE",
                nextQuestId = 1002,
                rewardMoney = 100,
                progress = 0
            });

            AddQuest(new QuestData
            {
                questId = 1004,
                title = "Путь избранного",
                description = "Наставник Ирана ждёт тебя у алтаря. Выбери свой путь.",
                objectiveText = "Выбрать специализацию у Наставника",
                minLevel = 10,
                maxLevel = 10,
                zoneName = "Застава Стражей",
                category = "Спец",
                nextQuestId = 1005,
                isSpecChoice = true,
                rewardMoney = 0,
                progress = 0
            });

            RefreshQuestList();
        }

        void AddQuest(QuestData quest)
        {
            _quests[quest.questId] = quest;
        }

        void RefreshQuestList()
        {
            if (_questList != null)
            {
                _questList.itemsSource = new List<int>(_quests.Keys);
                _questList.Rebuild();
            }
        }

        void OnQuestSelected(IEnumerable<object> selectedItems)
        {
            foreach (var item in selectedItems)
            {
                int qid = (int)item;
                _selectedQuestId = qid;
                ShowQuestDetails(qid);
                break;
            }
        }

        void ShowQuestDetails(int questId)
        {
            if (!_quests.ContainsKey(questId)) return;

            var quest = _quests[questId];

            if (_questTitle != null)
                _questTitle.text = quest.title;

            if (_questDescription != null)
                _questDescription.text = quest.description;

            if (_objectiveText != null)
                _objectiveText.text = quest.objectiveText;

            if (_specChoiceButton != null)
            {
                _specChoiceButton.style.display = quest.isSpecChoice ? DisplayStyle.Flex : DisplayStyle.None;
            }
        }

        void OnTrackClicked()
        {
            if (_selectedQuestId == 0) return;
            Debug.Log($"Tracking quest {_selectedQuestId}");
        }

        void OnAbandonClicked()
        {
            if (_selectedQuestId == 0) return;
            Debug.Log($"Abandoning quest {_selectedQuestId}");
            _quests.Remove(_selectedQuestId);
            _selectedQuestId = 0;
            RefreshQuestList();
        }

        void OnSpecChoiceClicked()
        {
            if (_selectedQuestId == 0) return;
            Debug.Log($"Opening spec choice for quest {_selectedQuestId}");
        }

        public void SetNetworkClient(MMONetworkClient client)
        {
            // Store reference for future use
        }

        public void UpdateQuestProgress(int questId, int progress)
        {
            if (!_quests.ContainsKey(questId)) return;

            _quests[questId].progress = progress;
            _quests[questId].objectiveText = $"Прогресс: {progress}";

            if (_selectedQuestId == questId)
                ShowQuestDetails(questId);

            RefreshQuestList();
        }
    }
}
