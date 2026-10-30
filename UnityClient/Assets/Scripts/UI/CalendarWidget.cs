using UnityEngine;
using UnityEngine.UIElements;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class CalendarWidget : MonoBehaviour
    {
        [SerializeField] private UIDocument uiDocument;

        private Label monthYearLabel;
        private Label currentDateLabel;
        private Label currentTimeLabel;
        private VisualElement calendarGrid;
        private ListView eventList;
        private Button prevMonthButton;
        private Button nextMonthButton;

        private DateTime currentMonth;
        private List<CalendarEventData> events = new List<CalendarEventData>();
        private DateTime lastRefreshDate = DateTime.MinValue;

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            monthYearLabel = root.Q<Label>("month-year-label");
            currentDateLabel = root.Q<Label>("current-date-label");
            currentTimeLabel = root.Q<Label>("current-time-label");
            calendarGrid = root.Q<VisualElement>("calendar-grid");
            eventList = root.Q<ListView>("event-list");
            prevMonthButton = root.Q<Button>("prev-month-button");
            nextMonthButton = root.Q<Button>("next-month-button");

            if (prevMonthButton != null) prevMonthButton.clicked += OnPrevMonthClicked;
            if (nextMonthButton != null) nextMonthButton.clicked += OnNextMonthClicked;

            currentMonth = DateTime.Now;
            LoadEvents();
            RefreshCalendar();
            UpdateTime();

            // Check for Monday 00:01 refresh
            InvokeRepeating("CheckWeeklyRefresh", 1f, 1f);
            InvokeRepeating("UpdateTime", 1f, 1f);
        }


        private void CheckWeeklyRefresh()
        {
            DateTime now = DateTime.Now;
            // Check if it's Monday 00:01
            if (now.DayOfWeek == DayOfWeek.Monday && now.Hour == 0 && now.Minute == 1)
            {
                if (lastRefreshDate.Date != now.Date)
                {
                    lastRefreshDate = now;
                    // Refresh calendar and events
                    LoadEvents();
                    RefreshCalendar();
                    UpdateTime();
                    Debug.Log("Calendar refreshed: Monday 00:01 weekly reset");
                }
            }
        }

        private void OnDisable()
        {
            if (prevMonthButton != null) prevMonthButton.clicked -= OnPrevMonthClicked;
            if (nextMonthButton != null) nextMonthButton.clicked -= OnNextMonthClicked;
            CancelInvoke("UpdateTime");
            CancelInvoke("CheckWeeklyRefresh");
        }

        private void UpdateTime()
        {
            if (currentDateLabel != null)
                currentDateLabel.text = DateTime.Now.ToString("dd.MM.yyyy");
            if (currentTimeLabel != null)
                currentTimeLabel.text = DateTime.Now.ToString("HH:mm:ss");
        }

        private void LoadEvents()
        {
            events.Clear();
            events.Add(new CalendarEventData { Id = 1, NameRu = "Новый Год", NameEn = "New Year", Day = 1, Month = 1, ColorHex = "#FF0000" });
            events.Add(new CalendarEventData { Id = 2, NameRu = "День Влюблённых", NameEn = "Valentine Day", Day = 14, Month = 2, ColorHex = "#FF69B4" });
            events.Add(new CalendarEventData { Id = 3, NameRu = "Хэллоуин", NameEn = "Halloween", Day = 31, Month = 10, ColorHex = "#800080" });
            events.Add(new CalendarEventData { Id = 9, NameRu = "Битва Духов", NameEn = "Spirit Battle", Day = DateTime.Now.Day, Month = DateTime.Now.Month, ColorHex = "#FF4500" });
            // Instances
            events.Add(new CalendarEventData { Id = 11, NameRu = "Подземелье Забытых", NameEn = "Dungeon of the Forgotten", Day = 1, Month = 1, ColorHex = "#8B4513" });
            events.Add(new CalendarEventData { Id = 14, NameRu = "Рейд: Цитадель Тьмы", NameEn = "Raid: Citadel of Darkness", Day = 1, Month = 1, ColorHex = "#4B0082" });
            events.Add(new CalendarEventData { Id = 15, NameRu = "Рейд: Логово Дракона", NameEn = "Raid: Dragon Lair", Day = 1, Month = 1, ColorHex = "#8B0000" });
            events.Add(new CalendarEventData { Id = 16, NameRu = "Инстанс: Разлом", NameEn = "Instance: Rift", Day = 1, Month = 1, ColorHex = "#4169E1" });
            events.Add(new CalendarEventData { Id = 18, NameRu = "Misty Anomaly", NameEn = "Misty Anomaly", Day = DateTime.Now.Day, Month = DateTime.Now.Month, ColorHex = "#4682B4" });
            // Seasonal
            events.Add(new CalendarEventData { Id = 29, NameRu = "Сезонное: Весна", NameEn = "Seasonal: Spring", Day = 1, Month = 3, ColorHex = "#00FF00" });
            events.Add(new CalendarEventData { Id = 30, NameRu = "Сезонное: Лето", NameEn = "Seasonal: Summer", Day = 1, Month = 6, ColorHex = "#FFD700" });
            events.Add(new CalendarEventData { Id = 31, NameRu = "Сезонное: Осень", NameEn = "Seasonal: Autumn", Day = 1, Month = 9, ColorHex = "#FF8C00" });
            events.Add(new CalendarEventData { Id = 32, NameRu = "Сезонное: Зима", NameEn = "Seasonal: Winter", Day = 1, Month = 12, ColorHex = "#00BFFF" });
        }

        private void RefreshCalendar()
        {
            if (monthYearLabel != null)
                monthYearLabel.text = currentMonth.ToString("MMMM yyyy");

            if (calendarGrid == null) return;

            calendarGrid.Clear();

            DateTime firstDay = new DateTime(currentMonth.Year, currentMonth.Month, 1);
            int startDayOfWeek = (int)firstDay.DayOfWeek;
            if (startDayOfWeek == 0) startDayOfWeek = 7;
            startDayOfWeek--;

            int daysInMonth = DateTime.DaysInMonth(currentMonth.Year, currentMonth.Month);

            string[] dayNames = { "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс" };
            for (int i = 0; i < 7; i++)
            {
                var header = new Label(dayNames[i]);
                header.AddToClassList("calendar-header");
                calendarGrid.Add(header);
            }

            for (int i = 0; i < startDayOfWeek; i++)
            {
                var empty = new Label("");
                empty.AddToClassList("calendar-empty");
                calendarGrid.Add(empty);
            }

            for (int day = 1; day <= daysInMonth; day++)
            {
                var dayLabel = new Label(day.ToString());
                dayLabel.AddToClassList("calendar-day");

                if (day == DateTime.Now.Day && currentMonth.Month == DateTime.Now.Month && currentMonth.Year == DateTime.Now.Year)
                {
                    dayLabel.AddToClassList("calendar-today");
                }

                DateTime date = new DateTime(currentMonth.Year, currentMonth.Month, day);
                if (date.DayOfWeek == DayOfWeek.Saturday || date.DayOfWeek == DayOfWeek.Sunday)
                {
                    dayLabel.AddToClassList("calendar-weekend");
                }

                calendarGrid.Add(dayLabel);
            }

            RefreshEventList();
        }

        private void RefreshEventList()
        {
            if (eventList == null) return;

            var monthEvents = events.Where(e => e.Month == currentMonth.Month).ToList();
            var items = monthEvents.Select(e => e.Day.ToString("D2") + "." + e.Month.ToString("D2") + " - " + e.NameRu).ToList();
            eventList.itemsSource = items;
            eventList.makeItem = () => new Label();
            eventList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
        }

        private void OnPrevMonthClicked()
        {
            currentMonth = currentMonth.AddMonths(-1);
            RefreshCalendar();
        }

        private void OnNextMonthClicked()
        {
            currentMonth = currentMonth.AddMonths(1);
            RefreshCalendar();
        }

        public void SetServerTime(int year, int month, int day, int hour, int minute, int second)
        {
            if (currentDateLabel != null)
                currentDateLabel.text = day.ToString("D2") + "." + month.ToString("D2") + "." + year;
            if (currentTimeLabel != null)
                currentTimeLabel.text = hour.ToString("D2") + ":" + minute.ToString("D2") + ":" + second.ToString("D2");

            // Update current month display if server date differs
            DateTime serverDate = new DateTime(year, month, day);
            if (currentMonth.Month != serverDate.Month || currentMonth.Year != serverDate.Year)
            {
                currentMonth = serverDate;
                RefreshCalendar();
            }
        }

        public void SyncWithServer()
        {
            // TODO: Send CMSG_CALENDAR_QUERY to server
            // Server responds with SMSG_CALENDAR_DATA containing:
            // - Current server time (year, month, day, hour, minute, second)
            // - List of active events for current month
            Debug.Log("Syncing calendar with server...");
        }
    }

    public class CalendarEventData
    {
        public int Id;
        public string NameRu;
        public string NameEn;
        public int Day;
        public int Month;
        public string ColorHex;
    }
}