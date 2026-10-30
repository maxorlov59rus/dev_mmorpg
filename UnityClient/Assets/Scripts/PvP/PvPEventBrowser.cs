// PvPEventBrowser.cs — Браузер PvP событий (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;

namespace DM.Client
{
    public class PvPEventBrowser : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _root;
        private ListView _eventList;
        private Label _eventName;
        private Label _eventDescription;
        private Label _eventSchedule;
        private Label _eventRewards;
        private Button _joinButton;
        private Button _closeButton;

        private int _selectedEventId = 0;

        [System.Serializable]
        public class PvPEvent
        {
            public int id;
            public string nameRu;
            public string descriptionRu;
            public int modeType;
            public int minPlayers;
            public int maxPlayers;
            public int teamSize;
            public int durationMin;
            public int dayOfWeek;
            public int startHour;
            public int rewardAmount;
            public int bonusRewardAmount;
            public int minLevel;
            public int maxLevel;
        }

        private List<PvPEvent> _events = new List<PvPEvent>();

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                _root = _uiDocument.rootVisualElement;
                _eventList = _root.Q<ListView>("EventList");
                _eventName = _root.Q<Label>("EventName");
                _eventDescription = _root.Q<Label>("EventDescription");
                _eventSchedule = _root.Q<Label>("EventSchedule");
                _eventRewards = _root.Q<Label>("EventRewards");
                _joinButton = _root.Q<Button>("JoinButton");
                _closeButton = _root.Q<Button>("CloseButton");

                if (_joinButton != null)
                    _joinButton.clicked += OnJoinClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;

                if (_eventList != null)
                {
                    _eventList.makeItem = () => new Label();
                    _eventList.bindItem = (element, index) =>
                    {
                        var label = element as Label;
                        if (index < _events.Count)
                            label.text = _events[index].nameRu;
                    };
                    _eventList.selectionChanged += OnEventSelected;
                }

                LoadEvents();
            }
        }
        void OnDisable()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        void LoadEvents()
        {
            _events.Add(new PvPEvent { id = 1, nameRu = "Арена 1v1", descriptionRu = "Дуэль 1 на 1", modeType = 1, minPlayers = 2, maxPlayers = 2, teamSize = 1, durationMin = 10, dayOfWeek = 1, startHour = 18, rewardAmount = 50, bonusRewardAmount = 200, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 2, nameRu = "Арена 3v3", descriptionRu = "Командная арена 3 на 3", modeType = 2, minPlayers = 6, maxPlayers = 6, teamSize = 3, durationMin = 15, dayOfWeek = 2, startHour = 18, rewardAmount = 100, bonusRewardAmount = 500, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 3, nameRu = "Поле боя 10v10", descriptionRu = "Большое поле боя", modeType = 2, minPlayers = 20, maxPlayers = 20, teamSize = 10, durationMin = 20, dayOfWeek = 3, startHour = 19, rewardAmount = 75, bonusRewardAmount = 300, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 4, nameRu = "Осада цитадели", descriptionRu = "Гильдии штурмуют цитадель", modeType = 3, minPlayers = 40, maxPlayers = 40, teamSize = 20, durationMin = 30, dayOfWeek = 4, startHour = 20, rewardAmount = 200, bonusRewardAmount = 1000, minLevel = 20, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 5, nameRu = "Фронтальная линия", descriptionRu = "3 фракции сражаются за контроль", modeType = 4, minPlayers = 60, maxPlayers = 60, teamSize = 1, durationMin = 45, dayOfWeek = 5, startHour = 18, rewardAmount = 150, bonusRewardAmount = 750, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 6, nameRu = "Караван Порога", descriptionRu = "Защита/атака каравана", modeType = 5, minPlayers = 30, maxPlayers = 30, teamSize = 1, durationMin = 25, dayOfWeek = 6, startHour = 17, rewardAmount = 100, bonusRewardAmount = 500, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 7, nameRu = "Дуэльный турнир", descriptionRu = "Турнир 1v1", modeType = 1, minPlayers = 16, maxPlayers = 16, teamSize = 1, durationMin = 30, dayOfWeek = 0, startHour = 16, rewardAmount = 300, bonusRewardAmount = 1500, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 8, nameRu = "Захват флага", descriptionRu = "Команды захватывают флаг", modeType = 2, minPlayers = 10, maxPlayers = 10, teamSize = 5, durationMin = 15, dayOfWeek = 3, startHour = 20, rewardAmount = 80, bonusRewardAmount = 400, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 9, nameRu = "Королевская битва", descriptionRu = "100 игроков, последний выживший", modeType = 1, minPlayers = 100, maxPlayers = 100, teamSize = 1, durationMin = 20, dayOfWeek = 6, startHour = 21, rewardAmount = 500, bonusRewardAmount = 2500, minLevel = 10, maxLevel = 60 });
            _events.Add(new PvPEvent { id = 10, nameRu = "Арена 2v2", descriptionRu = "Командная арена 2 на 2", modeType = 2, minPlayers = 4, maxPlayers = 4, teamSize = 2, durationMin = 10, dayOfWeek = 4, startHour = 18, rewardAmount = 60, bonusRewardAmount = 300, minLevel = 10, maxLevel = 60 });

            if (_eventList != null)
            {
                _eventList.itemsSource = _events;
                _eventList.Rebuild();
            }
        }

        void OnEventSelected(IEnumerable<object> selectedItems)
        {
            foreach (var item in selectedItems)
            {
                var evt = item as PvPEvent;
                if (evt != null)
                {
                    _selectedEventId = evt.id;
                    ShowEventDetails(evt);
                }
                break;
            }
        }

        void ShowEventDetails(PvPEvent evt)
        {
            if (_eventName != null)
                _eventName.text = evt.nameRu;
            if (_eventDescription != null)
                _eventDescription.text = evt.descriptionRu;
            if (_eventSchedule != null)
            {
                string[] days = { "Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб" };
                _eventSchedule.text = $"Расписание: {days[evt.dayOfWeek]} {evt.startHour}:00, {evt.durationMin} мин";
            }
            if (_eventRewards != null)
                _eventRewards.text = $"Награда: {evt.rewardAmount} золота (победа: {evt.bonusRewardAmount})";
        }

        void OnJoinClicked()
        {
            if (_selectedEventId == 0) return;
            Debug.Log($"Joining PvP event {_selectedEventId}");
        }

        void OnCloseClicked()
        {
            gameObject.SetActive(false);
        }

        void OnDestroy()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}