// SeasonalEventClient.cs — Клиентская система сезонных эвентов (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;

namespace DM.Client
{
    public class SeasonalEventClient : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _root;
        private Label _eventName;
        private Label _eventDescription;
        private Label _currencyLabel;
        private ListView _questList;
        private ListView _shopList;
        private Button _joinButton;
        private Button _shopButton;
        private Button _closeButton;

        private int _playerCurrency = 0;

        [System.Serializable]
        public class SeasonalQuest
        {
            public int id;
            public string titleRu;
            public string descriptionRu;
            public string objectiveRu;
            public int rewardCurrency;
            public int rewardItemId;
            public int minLevel;
        }

        [System.Serializable]
        public class SeasonalShopItem
        {
            public int id;
            public string nameRu;
            public string descriptionRu;
            public int currencyCost;
            public int requiredLevel;
            public int itemLevel;
            public string uniqueStats;
        }

        private List<SeasonalQuest> _quests = new List<SeasonalQuest>();
        private List<SeasonalShopItem> _shopItems = new List<SeasonalShopItem>();

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                _root = _uiDocument.rootVisualElement;
                _eventName = _root.Q<Label>("EventName");
                _eventDescription = _root.Q<Label>("EventDescription");
                _currencyLabel = _root.Q<Label>("CurrencyLabel");
                _questList = _root.Q<ListView>("QuestList");
                _shopList = _root.Q<ListView>("ShopList");
                _joinButton = _root.Q<Button>("JoinButton");
                _shopButton = _root.Q<Button>("ShopButton");
                _closeButton = _root.Q<Button>("CloseButton");

                if (_joinButton != null)
                    _joinButton.clicked += OnJoinClicked;
                if (_shopButton != null)
                    _shopButton.clicked += OnShopClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                LoadSeasonalData();
            }
        }

        void OnDisable()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_shopButton != null)
                _shopButton.clicked -= OnShopClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        void LoadSeasonalData()
        {
            // Sample seasonal quests
            _quests.Add(new SeasonalQuest { id = 1, titleRu = "Цветение Рассвета", descriptionRu = "Собери весенние цветы для праздника.", objectiveRu = "Собрать 10 весенних цветов", rewardCurrency = 20, rewardItemId = 16001, minLevel = 10 });
            _quests.Add(new SeasonalQuest { id = 2, titleRu = "Свет Рассвета", descriptionRu = "Зажги фонари в столице.", objectiveRu = "Зажечь 5 фонарей", rewardCurrency = 15, rewardItemId = 16002, minLevel = 10 });
            _quests.Add(new SeasonalQuest { id = 3, titleRu = "Песнь Рассвета", descriptionRu = "Спой песнь рассвета у алтаря.", objectiveRu = "Спеть песнь рассвета", rewardCurrency = 25, rewardItemId = 16003, minLevel = 15 });

            // Sample shop items
            _shopItems.Add(new SeasonalShopItem { id = 1, nameRu = "Венок Рассвета", descriptionRu = "Праздничный венок из весенних цветов.", currencyCost = 50, requiredLevel = 1, itemLevel = 1, uniqueStats = "Косметический предмет" });
            _shopItems.Add(new SeasonalShopItem { id = 2, nameRu = "Фонарь Рассвета", descriptionRu = "Светящийся фонарь для праздника.", currencyCost = 75, requiredLevel = 5, itemLevel = 5, uniqueStats = "Косметический предмет" });
            _shopItems.Add(new SeasonalShopItem { id = 3, nameRu = "Крылья Рассвета", descriptionRu = "Косметические крылья рассвета.", currencyCost = 500, requiredLevel = 20, itemLevel = 20, uniqueStats = "Косметический предмет" });

            RefreshUI();
        }

        void RefreshUI()
        {
            if (_currencyLabel != null)
                _currencyLabel.text = $"Сезонная валюта: {_playerCurrency}";

            if (_questList != null)
            {
                _questList.itemsSource = _quests;
                _questList.Rebuild();
            }

            if (_shopList != null)
            {
                _shopList.itemsSource = _shopItems;
                _shopList.Rebuild();
            }
        }

        void OnJoinClicked()
        {
            Debug.Log("Joining seasonal event...");
        }

        void OnShopClicked()
        {
            Debug.Log("Opening seasonal shop...");
        }

        void OnCloseClicked()
        {
            gameObject.SetActive(false);
        }

        void OnDestroy()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_shopButton != null)
                _shopButton.clicked -= OnShopClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}