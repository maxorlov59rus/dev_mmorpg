// AutoPvPClient.cs — Клиентская система авто-PvP (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;

namespace DM.Client
{
    public class AutoPvPClient : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _root;
        private ListView _difficultyList;
        private Label _arenaName;
        private Label _difficultyName;
        private Label _cloneInfo;
        private Label _faithPoints;
        private Label _matchHistory;
        private Button _startMatchButton;
        private Button _shopButton;
        private Button _closeButton;

        private int _selectedDifficulty = 1;

        [System.Serializable]
        public class AutoPvPDifficulty
        {
            public int id;
            public string nameRu;
            public float statMultiplier;
            public int aiLevel;
            public int faithReward;
            public int bonusFaith;
            public int minLevel;
            public int entryFee;
        }

        [System.Serializable]
        public class FaithShopItem
        {
            public int id;
            public string nameRu;
            public int faithCost;
            public int requiredLevel;
            public int requiredDifficulty;
            public int itemLevel;
            public string uniqueStats;
            public int stock;
        }

        private List<AutoPvPDifficulty> _difficulties = new List<AutoPvPDifficulty>();
        private List<FaithShopItem> _shopItems = new List<FaithShopItem>();

        private int _playerFaithPoints = 0;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                _root = _uiDocument.rootVisualElement;
                _arenaName = _root.Q<Label>("ArenaName");
                _difficultyList = _root.Q<ListView>("DifficultyList");
                _difficultyName = _root.Q<Label>("DifficultyName");
                _cloneInfo = _root.Q<Label>("CloneInfo");
                _faithPoints = _root.Q<Label>("FaithPoints");
                _matchHistory = _root.Q<Label>("MatchHistory");
                _startMatchButton = _root.Q<Button>("StartMatchButton");
                _shopButton = _root.Q<Button>("ShopButton");
                _closeButton = _root.Q<Button>("CloseButton");

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

                LoadDifficulties();
                LoadShopItems();
                UpdateFaithPoints();
            }
        }

        void OnDisable()
        {
            if (_startMatchButton != null)
                _startMatchButton.clicked -= OnStartMatchClicked;
            if (_shopButton != null)
                _shopButton.clicked -= OnShopClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        void LoadDifficulties()
        {
            _difficulties.Add(new AutoPvPDifficulty { id = 1, nameRu = "Лёгкий", statMultiplier = 0.8f, aiLevel = 1, faithReward = 10, bonusFaith = 5, minLevel = 10, entryFee = 0 });
            _difficulties.Add(new AutoPvPDifficulty { id = 2, nameRu = "Средний", statMultiplier = 1.0f, aiLevel = 2, faithReward = 25, bonusFaith = 10, minLevel = 20, entryFee = 50 });
            _difficulties.Add(new AutoPvPDifficulty { id = 3, nameRu = "Сложный", statMultiplier = 1.2f, aiLevel = 3, faithReward = 50, bonusFaith = 25, minLevel = 30, entryFee = 100 });

            if (_difficultyList != null)
            {
                _difficultyList.itemsSource = _difficulties;
                _difficultyList.Rebuild();
            }
        }

        void LoadShopItems()
        {
            _shopItems.Add(new FaithShopItem { id = 1, nameRu = "Клинок Призрака", faithCost = 100, requiredLevel = 10, requiredDifficulty = 1, itemLevel = 15, uniqueStats = "+10% урон по клонам", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 2, nameRu = "Посох Призрака", faithCost = 150, requiredLevel = 15, requiredDifficulty = 1, itemLevel = 20, uniqueStats = "+15% магический урон", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 3, nameRu = "Лук Призрака", faithCost = 120, requiredLevel = 12, requiredDifficulty = 1, itemLevel = 18, uniqueStats = "+12% скорость атаки", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 4, nameRu = "Кинжал Призрака", faithCost = 200, requiredLevel = 20, requiredDifficulty = 2, itemLevel = 25, uniqueStats = "+20% критический урон", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 5, nameRu = "Молот Призрака", faithCost = 300, requiredLevel = 25, requiredDifficulty = 2, itemLevel = 30, uniqueStats = "+25% урон по элитным клонам", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 6, nameRu = "Скипетр Призрака", faithCost = 250, requiredLevel = 20, requiredDifficulty = 2, itemLevel = 28, uniqueStats = "+30% исцеление", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 7, nameRu = "Коса Призрака", faithCost = 400, requiredLevel = 30, requiredDifficulty = 3, itemLevel = 35, uniqueStats = "+15% урон, +10% вампиризм", stock = -1 });
            _shopItems.Add(new FaithShopItem { id = 8, nameRu = "Клинок Пустоты", faithCost = 1000, requiredLevel = 40, requiredDifficulty = 3, itemLevel = 45, uniqueStats = "+50% урон, +20% крит", stock = 10 });
            _shopItems.Add(new FaithShopItem { id = 9, nameRu = "Посох Пустоты", faithCost = 1000, requiredLevel = 40, requiredDifficulty = 3, itemLevel = 45, uniqueStats = "+60% магический урон", stock = 10 });
            _shopItems.Add(new FaithShopItem { id = 10, nameRu = "Лук Пустоты", faithCost = 1000, requiredLevel = 40, requiredDifficulty = 3, itemLevel = 45, uniqueStats = "+40% скорость атаки, +25% крит", stock = 10 });
        }

        void OnDifficultySelected(IEnumerable<object> selectedItems)
        {
            foreach (var item in selectedItems)
            {
                var diff = item as AutoPvPDifficulty;
                if (diff != null)
                {
                    _selectedDifficulty = diff.id;
                    ShowDifficultyDetails(diff);
                }
                break;
            }
        }

        void ShowDifficultyDetails(AutoPvPDifficulty diff)
        {
            if (_difficultyName != null)
                _difficultyName.text = diff.nameRu;
            if (_cloneInfo != null)
                _cloneInfo.text = $"Множитель статов: {diff.statMultiplier}x, ИИ: {diff.aiLevel}, Награда: {diff.faithReward} очков веры";
        }

        void UpdateFaithPoints()
        {
            if (_faithPoints != null)
                _faithPoints.text = $"Очки веры: {_playerFaithPoints}";
        }

        void OnStartMatchClicked()
        {
            var diff = _difficulties.Find(d => d.id == _selectedDifficulty);
            if (diff == null) return;

            Debug.Log($"Starting auto-PvP match: difficulty={diff.nameRu}");
        }

        void OnShopClicked()
        {
            Debug.Log("Opening faith shop...");
        }

        void OnCloseClicked()
        {
            gameObject.SetActive(false);
        }

        void OnDestroy()
        {
            if (_startMatchButton != null)
                _startMatchButton.clicked -= OnStartMatchClicked;
            if (_shopButton != null)
                _shopButton.clicked -= OnShopClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}
