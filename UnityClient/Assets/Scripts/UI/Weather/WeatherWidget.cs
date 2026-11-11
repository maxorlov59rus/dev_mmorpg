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
        /// </summary>
        public void UpdateWeatherDisplay()
        {
            bool hasWeather = weatherContainer != null && weatherContainer.activeSelf;

            if (noWeatherContainer != null)
                noWeatherContainer.SetActive(!hasWeather);

            if (!hasWeather)
                return;

            if (weatherTypeText != null && string.IsNullOrEmpty(weatherTypeText.text))
                weatherTypeText.text = "Ясно";

            if (intensityText != null && string.IsNullOrEmpty(intensityText.text))
                intensityText.text = "-";

            if (temperatureText != null && string.IsNullOrEmpty(temperatureText.text))
                temperatureText.text = "0°C";

            if (humidityText != null && string.IsNullOrEmpty(humidityText.text))
                humidityText.text = "-";

            if (windText != null && string.IsNullOrEmpty(windText.text))
                windText.text = "-";
        }

        public void SetWeather(string type, string intensity, float temperature, float humidity, float wind)
        {
            if (weatherContainer != null) weatherContainer.SetActive(true);
            if (noWeatherContainer != null) noWeatherContainer.SetActive(false);

            if (weatherTypeText != null) weatherTypeText.text = type;
            if (intensityText != null) intensityText.text = intensity;
            if (temperatureText != null) temperatureText.text = Mathf.RoundToInt(temperature) + "°C";
            if (humidityText != null) humidityText.text = Mathf.RoundToInt(humidity) + "%";
            if (windText != null) windText.text = wind.ToString("0.0");
        }

        public void ClearWeather()
        {
            if (weatherContainer != null) weatherContainer.SetActive(false);
            if (noWeatherContainer != null) noWeatherContainer.SetActive(true);
        }
    }
}
