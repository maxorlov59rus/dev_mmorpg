// WeatherIndicator.cs
// Displays current weather conditions with visual indicators on the HUD using UI Toolkit.
using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;

namespace DM.Client
{
    /// <summary>
    /// Visual weather indicator that displays weather type, severity, and updates periodically.
    /// </summary>
    public class WeatherIndicator : MonoBehaviour
    {
        [Header("Weather Settings")]
        [Tooltip("Weather icon elements (Clear, Rain, Thunder, Snow, Fog, Sandstorm)")]
        public VisualTreeAsset weatherUxml;

        private VisualElement _weatherIcon;
        private Label _weatherLabel;
        private Label _severityLabel;
        private UIDocument _uiDocument;

        private byte _currentWeatherType = 0;
        private byte _currentSeverity = 0;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _weatherIcon = root.Q<VisualElement>("WeatherIcon");
                _weatherLabel = root.Q<Label>("WeatherText");
                _severityLabel = root.Q<Label>("SeverityText");
            }

            UpdateWeatherDisplay(0, 0, "Clear");
        }

        /// <summary>
        /// Updates the weather display with new conditions.
        /// </summary>
        public void UpdateWeatherDisplay(byte weatherType, byte severity, string weatherName = null)
        {
            _currentWeatherType = weatherType;
            _currentSeverity = severity;

            if (_weatherIcon != null)
            {
                _weatherIcon.RemoveFromClassList("weather-clear");
                _weatherIcon.RemoveFromClassList("weather-rain");
                _weatherIcon.RemoveFromClassList("weather-thunder");
                _weatherIcon.RemoveFromClassList("weather-snow");
                _weatherIcon.RemoveFromClassList("weather-fog");
                _weatherIcon.RemoveFromClassList("weather-sandstorm");

                string className = GetWeatherClassName(weatherType);
                _weatherIcon.AddToClassList(className);
            }

            if (_weatherLabel != null)
                _weatherLabel.text = weatherName ?? GetWeatherName(weatherType);

            if (_severityLabel != null)
                _severityLabel.text = GetSeverityName(severity);

            Debug.Log($"Weather updated: {GetWeatherName(weatherType)} ({GetSeverityName(severity)})");
        }

        /// <summary>
        /// Updates weather from a WeatherUpdatePacket received from the server.
        /// </summary>
        public void UpdateFromServer(WeatherUpdatePacket packet)
        {
            string weatherName = GetWeatherName(packet.weatherType);
            UpdateWeatherDisplay(packet.weatherType, packet.severity, weatherName);

            if (packet.transition == 1)
                Debug.Log($"Weather transitioning over {packet.durationMs}ms");
        }

        /// <summary>
        /// Updates weather from a TimeSyncPacket (which includes weather info).
        /// </summary>
        public void UpdateFromServer(TimeSyncPacket packet)
        {
            string weatherName = GetWeatherName(packet.weatherType);
            UpdateWeatherDisplay(packet.weatherType, packet.weatherSeverity, weatherName);
        }

        string GetWeatherName(byte weatherType)
        {
            switch (weatherType)
            {
                case 0: return "Clear";
                case 1: return "Rain";
                case 2: return "Thunderstorm";
                case 3: return "Snow";
                case 4: return "Fog";
                case 5: return "Sandstorm";
                default: return "Unknown";
            }
        }

        string GetSeverityName(byte severity)
        {
            switch (severity)
            {
                case 0: return "Light";
                case 1: return "Medium";
                case 2: return "Heavy";
                default: return "Unknown";
            }
        }

        string GetWeatherClassName(byte weatherType)
        {
            switch (weatherType)
            {
                case 0: return "weather-clear";
                case 1: return "weather-rain";
                case 2: return "weather-thunder";
                case 3: return "weather-snow";
                case 4: return "weather-fog";
                case 5: return "weather-sandstorm";
                default: return "weather-clear";
            }
        }
    }
}
