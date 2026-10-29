// VendorWidget.cs — UI вендора
using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;

namespace DarkMatter.UI
{
    public class VendorWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;
        private ListView itemList;
        private Label moneyLabel;

        void OnEnable()
        {
            root = GetComponent<UIDocument>().rootVisualElement;
            itemList = root.Q<ListView>("ItemList");
            moneyLabel = root.Q<Label>("MoneyLabel");
        }

        public void UpdateVendorList(uint npcGuid, (uint itemId, uint price, byte stack)[] items)
        {
            itemList.itemsSource = items;
            itemList.makeItem = () => new Label();
            itemList.bindItem = (ve, i) =>
            {
                var label = (Label)ve;
                label.text = "Item " + items[i].itemId + " - " + items[i].price + "g (x" + items[i].stack + ")";
            };
        }

        public void UpdateMoney(uint money)
        {
            moneyLabel.text = "Gold: " + money;
        }
    }
}
