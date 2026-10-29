// WorldMapWidget.cs — заглушка UI карты мира
using UnityEngine;
using UnityEngine.UIElements;

namespace DarkMatter.UI
{
    public class WorldMapWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;

        void OnEnable()
        {
            root = GetComponent<UIDocument>().rootVisualElement;
            // TODO: реализовать карту локаций
        }
    }
}
