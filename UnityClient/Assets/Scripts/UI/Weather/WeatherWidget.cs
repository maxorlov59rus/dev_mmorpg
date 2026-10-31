using UnityEngine;
using UnityEngine.UI;

namespace BlackMateria.UI.Weather
{
    /// <summary>
    /// Weather widget displaying current weather conditions
    /// </summary>
    public class WeatherWidget : MonoBehaviour
    {
        [Header("Weather UI Elements")]
        [SerializeField] private Text weatherTypeText;
        [SerializeField] private Text intensityText;
        [SerializeField] private Image weatherIcon;
        [SerializeField] private Text temperatureText;
        [SerializeField] private Text humidityText;
        [SerializeField] private Text windText;
        [SerializeField] private GameObject weatherContainer;
        [SerializeField] private GameObject noWeatherContainer;

        private void Awake()
        {
            UpdateWeatherDisplay();
        }

        /// <summary>
        /// Update the weather display with new data
        ///