// LFGClient.cs — Клиентская система подбора в данжи (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;

namespace DM.Client
{
    public class LFGClient : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _root;
        private ListView _dungeonList;
        private ListView _difficultyList;
        private Label _dungeonName;
        private Label _dungeonDescription;
        private Label _dungeonInfo;
        private Label _queueStatus;
        private Button _joinQueueButton;
        private Button _leaveQueueButton;
        private Button _closeButton;

        private int _selectedDungeonId = 0;
        private int _selectedDifficulty = 0;

        [System.Serializable]
        public class DungeonInfo
        {
            public int id;
            public string nameRu;
            public int mapId;
            public int minLevel;
            public int maxLevel;
            public int minPlayers;
            public int maxPlayers;
            public bool isRaid;
            public int resetHours;
        }

        [System.Serializable]
        public class DungeonDifficulty
        {
            public int id;
            public string nameRu;
            public float hpMultiplier;
            public float dmgMultiplier;
            public float lootMultiplier;
            public float xpMultiplier;
            public int minItemLevel;
        }

        private List<DungeonInfo> _dungeons = new List<DungeonInfo>();
        private List<DungeonDifficulty> _difficulties = new List<DungeonDifficulty>();

        private bool _inQueue = false;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                _root = _uiDocument.rootVisualElement;
                _dungeonList = _root.Q<ListView>("DungeonList");
                _difficultyList = _root.Q<ListView>("DifficultyList");
                _dungeonName = _root.Q<Label>("DungeonName");
                _dungeonDescription = _root.Q<Label>("DungeonDescription");
                _dungeonInfo = _root.Q<Label>("DungeonInfo");
                _queueStatus = _root.Q<Label>("QueueStatus");
                _joinQueueButton = _root.Q<Button>("JoinQueueButton");
                _leaveQueueButton = _root.Q<Button>("LeaveQueueButton");
                _closeButton = _root.Q<Button>("CloseButton");

                if (_joinQueueButton != null)
                    _joinQueueButton.clicked += OnJoinQueueClicked;
                if (_leaveQueueButton != null)
                    _leaveQueueButton.clicked += OnLeaveQueueClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                if (_dungeonList != null)
                {
                    _dungeonList.makeItem = () => new Label();
                    _dungeonList.bindItem = (element, index) =>
                    {
                        var label = element as Label;
                        if (index < _dungeons.Count)
                            label.text = _dungeons[index].nameRu;
                    };
                    _dungeonList.selectionChanged += OnDungeonSelected;
                }

                if (_difficultyList != null)
                {
                    _difficultyList.makeItem = () => new Label();
                    _difficultyList.bindItem = (element, index) =>
                    {
                        var label = element as Label;
                        if (index < _difficulties.Count)
                            label.text = _difficulties[index].nameRu;
                    };
                    _difficultyList.selectionChanged += OnDifficultySelected;
                }

                LoadDungeons();
                LoadDifficulties();
            }
        }

        void OnDisable()
        {
            if (_joinQueueButton != null)
                _joinQueueButton.clicked -= OnJoinQueueClicked;
            if (_leaveQueueButton != null)
                _leaveQueueButton.clicked -= OnLeaveQueueClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        void LoadDungeons()
        {
            _dungeons.Add(new DungeonInfo { id = 20, nameRu = "Пещеры Испытаний", mapId = 20, minLevel = 10, maxLevel = 15, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 7, nameRu = "Храм забвения", mapId = 7, minLevel = 19, maxLevel = 22, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 9, nameRu = "Цитадель тьмы", mapId = 9, minLevel = 23, maxLevel = 26, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 11, nameRu = "Разлом Шип", mapId = 11, minLevel = 27, maxLevel = 31, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 12, nameRu = "Цитадель Искажённых", mapId = 12, minLevel = 32, maxLevel = 36, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 13, nameRu = "Мёртвые Пределы", mapId = 13, minLevel = 37, maxLevel = 41, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 14, nameRu = "Руины Древних", mapId = 14, minLevel = 42, maxLevel = 46, minPlayers = 1, maxPlayers = 5, isRaid = false, resetHours = 24 });
            _dungeons.Add(new DungeonInfo { id = 15, nameRu = "Глотка", mapId = 15, minLevel = 47, maxLevel = 50, minPlayers = 10, maxPlayers = 10, isRaid = true, resetHours = 168 });
            _dungeons.Add(new DungeonInfo { id = 100, nameRu = "Катакомбы Новичков", mapId = 100, minLevel = 1, maxLevel = 9, minPlayers = 1, maxPlayers = 3, isRaid = false, resetHours = 12 });
            _dungeons.Add(new DungeonInfo { id = 101, nameRu = "Логово Волков", mapId = 101, minLevel = 5, maxLevel = 10, minPlayers = 1, maxPlayers = 3, isRaid = false, resetHours = 12 });

            if (_dungeonList != null)
            {
                _dungeonList.itemsSource = _dungeons;
                _dungeonList.Rebuild();
            }
        }

        void LoadDifficulties()
        {
            _difficulties.Add(new DungeonDifficulty { id = 0, nameRu = "Обычный", hpMultiplier = 1.0f, dmgMultiplier = 1.0f, lootMultiplier = 1.0f, xpMultiplier = 1.0f, minItemLevel = 0 });
            _difficulties.Add(new DungeonDifficulty { id = 1, nameRu = "Героический", hpMultiplier = 2.0f, dmgMultiplier = 1.5f, lootMultiplier = 1.5f, xpMultiplier = 1.2f, minItemLevel = 30 });
            _difficulties.Add(new DungeonDifficulty { id = 2, nameRu = "Эпический", hpMultiplier = 4.0f, dmgMultiplier = 2.0f, lootMultiplier = 2.0f, xpMultiplier = 1.5f, minItemLevel = 50 });
            _difficulties.Add(new DungeonDifficulty { id = 3, nameRu = "Легендарный", hpMultiplier = 8.0f, dmgMultiplier = 3.0f, lootMultiplier = 3.0f, xpMultiplier = 2.0f, minItemLevel = 70 });

            if (_difficultyList != null)
            {
                _difficultyList.itemsSource = _difficulties;
                _difficultyList.Rebuild();
            }
        }

        void OnDungeonSelected(IEnumerable<object> selectedItems)
        {
            foreach (var item in selectedItems)
            {
                var dungeon = item as DungeonInfo;
                if (dungeon != null)
                {
                    _selectedDungeonId = dungeon.id;
                    ShowDungeonDetails(dungeon);
                }
                break;
            }
        }

        void OnDifficultySelected(IEnumerable<object> selectedItems)
        {
            foreach (var item in selectedItems)
            {
                var diff = item as DungeonDifficulty;
                if (diff != null)
                {
                    _selectedDifficulty = diff.id;
                }
                break;
            }
        }

        void ShowDungeonDetails(DungeonInfo dungeon)
        {
            if (_dungeonName != null)
                _dungeonName.text = dungeon.nameRu;
            if (_dungeonDescription != null)
                _dungeonDescription.text = dungeon.isRaid ? "Рейд (10 игроков)" : "Данж";
            if (_dungeonInfo != null)
                _dungeonInfo.text = $"Уровень: {dungeon.minLevel}-{dungeon.maxLevel}, Игроки: {dungeon.minPlayers}-{dungeon.maxPlayers}, Сброс: {dungeon.resetHours}ч";
        }

        void OnJoinQueueClicked()
        {
            if (_selectedDungeonId == 0) return;
            Debug.Log($"Joining LFG queue: dungeon={_selectedDungeonId}, difficulty={_selectedDifficulty}");
            _inQueue = true;
            if (_queueStatus != null)
                _queueStatus.text = "В очереди...";
        }

        void OnLeaveQueueClicked()
        {
            if (!_inQueue) return;
            Debug.Log("Leaving LFG queue");
            _inQueue = false;
            if (_queueStatus != null)
                _queueStatus.text = "Не в очереди";
        }

        void OnCloseClicked()
        {
            gameObject.SetActive(false);
        }

        void OnDestroy()
        {
            if (_joinQueueButton != null)
                _joinQueueButton.clicked -= OnJoinQueueClicked;
            if (_leaveQueueButton != null)
                _leaveQueueButton.clicked -= OnLeaveQueueClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}
