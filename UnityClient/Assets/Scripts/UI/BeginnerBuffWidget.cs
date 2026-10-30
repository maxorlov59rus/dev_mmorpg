using UnityEngine;
using UnityEngine.UIElements;
using System;

namespace DM.Client
{
    public class BeginnerBuffWidget : MonoBehaviour
    {
        [SerializeField] private UIDocument uiDocument;

        private VisualElement buffIcon;
        private Label buffNameLabel;
        private Label buffDescriptionLabel;
        private Label buffStatsLabel;
        private VisualElement buffContainer;

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            buffIcon = root.Q<VisualElement>("buff-icon");
            buffNameLabel = root.Q<Label>("buff-name");
            buffDescriptionLabel = root.Q<Label>("buff-description");
            buffStatsLabel = root.Q<Label>("buff-stats");
            buffContainer = root.Q<VisualElement>("buff-container");

            // Hide by default
            if (buffContainer != null) buffContainer.style.display = DisplayStyle.None;
        }

        public void ApplyBuff(int playerLevel)
        {
            if (playerLevel < 1 || playerLevel > 10) return;

            if (buffContainer != null) buffContainer.style.display = DisplayStyle.Flex;
            if (buffNameLabel != null) buffNameLabel.text = "Благословение Новичка";
            if (buffDescriptionLabel != null) buffDescriptionLabel.text = "Бафф для новых игроков";
            if (buffStatsLabel != null) buffStatsLabel.text = "+50% XP, +25% дроп, +25% золото, +10% скорость";

            Debug.Log($"Beginner buff applied for level {playerLevel}");
        }

        public void RemoveBuff()
        {
            if (buffContainer != null) buffContainer.style.display = DisplayStyle.None;

            Debug.Log("Beginner buff removed");
        }

        public void UpdateBuffStatus(bool active, int playerLevel)
        {
            if (active && playerLevel <= 10)
            {
                ApplyBuff(playerLevel);
            }
            else
            {
                RemoveBuff();
            }
        }
    }
}
