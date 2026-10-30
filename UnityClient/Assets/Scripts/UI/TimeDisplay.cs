// TimeDisplay.cs
// Shows the current game time on the HUD using UI Toolkit.
using UnityEngine;
using UnityEngine.UIElements;
using System;

namespace DM.Client
{
    /// <summary>
    /// Displays the current game time (or real time) in the UI.
    /// Updates once per second in either real-time mode or simulated game-time mode.
    /// </summary>
    public class TimeDisplay : MonoBehaviour
    {
        [Header("Settings")]
        [Tooltip("If true, uses simulated game time that cycles like a 24h day.")]
        public bool useServerTime = true;
        [Tooltip("Use 24-hour format instead of 12-hour with AM/PM.")]
        public bool show24HourFormat = false;

        // Internal timer tracking (simulated game day of 24 real-time minutes)
        private float _elapsedTime = 0f;
        private const float SIMULATED_DAY_LENGTH = 1440f; // 24 minutes real-time = 24 hours game time

        private Label _timeLabel;
        private Label _dateLabel;

        void OnEnable()
        {
            var root = GetComponent<UIDocument>()?.rootVisualElement;
            if (root != null)
            {
                _timeLabel = root.Q<Label>("TimeText");
                if (_timeLabel == null)
                    _timeLabel = root.Q<Label>("time-text");
                _dateLabel = root.Q<Label>("DateText");
                if (_dateLabel == null)
                    _dateLabel = root.Q<Label>("date-text");
            }

            if (_timeLabel != null)
                _timeLabel.text = "--:--";
            if (_dateLabel != null)
                _dateLabel.text = "--.--.----";

            // Start the updating routine
            InvokeRepeating("UpdateTime", 1f, 1f);
        }

        void OnDisable()
        {
            CancelInvoke("UpdateTime");
        }

        void UpdateTime()
        {
            string currentTime;
            string currentDate = DateTime.Now.ToString("dd.MM.yyyy");

            if (useServerTime)
            {
                _elapsedTime += 1f;

                int totalSeconds = (int)(_elapsedTime % SIMULATED_DAY_LENGTH);
                int hour = totalSeconds / 60;
                int minute = totalSeconds % 60;

                currentTime = FormatTime(hour, minute);
            }
            else
            {
                currentTime = DateTime.Now.ToString(show24HourFormat ? "HH:mm" : "hh:mm tt");
            }

            if (_timeLabel != null)
                _timeLabel.text = currentTime;
            if (_dateLabel != null)
                _dateLabel.text = currentDate;
        }

        string FormatTime(int hour, int minute)
        {
            if (show24HourFormat)
                return $"{hour:D2}:{minute:D2}";
            else
            {
                int h12 = hour % 12;
                h12 = h12 == 0 ? 12 : h12;
                string ampm = hour < 12 ? "AM" : "PM";
                return $"{h12:D2}:{minute:D2} {ampm}";
            }
        }

        /// <summary>
        /// Update the time display from a server sync packet.
        /// </summary>
        public void UpdateFromGameTime(int hour, int minute)
        {
            if (_timeLabel != null)
                _timeLabel.text = FormatTime(hour, minute);

            _elapsedTime = hour * 60f + minute;

            if (_dateLabel != null)
                _dateLabel.text = DateTime.Now.ToString("dd.MM.yyyy");
        }
    }
}
