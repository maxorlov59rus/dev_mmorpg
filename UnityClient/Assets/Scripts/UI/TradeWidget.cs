// TradeWidget.cs — UI обмена между игроками
using UnityEngine;
using UnityEngine.UIElements;

namespace DarkMatter.UI
{
    public class TradeWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;
        private ListView playerAItems;
        private ListView playerBItems;
        private Label statusLabel;

        void OnEnable()
        {
            root = GetComponent<UIDocument>().rootVisualElement;
            playerAItems = root.Q<ListView>("PlayerAItems");
            playerBItems = root.Q<ListView>("PlayerBItems");
            statusLabel = root.Q<Label>("StatusLabel");
        }

        public void UpdateTradeState(bool acceptedA, bool acceptedB)
        {
            statusLabel.text = "A: " + (acceptedA ? "Accepted" : "Pending") + " | B: " + (acceptedB ? "Accepted" : "Pending");
        }
    }
}
