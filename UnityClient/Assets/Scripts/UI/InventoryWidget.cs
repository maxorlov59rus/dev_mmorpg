// InventoryWidget.cs — заглушка UI инвентаря
using UnityEngine;
using UnityEngine.UIElements;

namespace DarkMatter.UI
{
    public class InventoryWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;

        void OnEnable()
        {
            root = GetComponent<UIDocument>().rootVisualElement;
            // TODO: реализовать сетку предметов
        }
    }
}
