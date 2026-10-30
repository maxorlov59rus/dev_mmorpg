using UnityEngine;
using UnityEngine.UIElements;
using DarkMatter.Network;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class SpiritWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        [SerializeField] private UIDocument uiDocument;

        private ListView spiritList;
        private Label spiritNameLabel;
        private Label spiritQualityLabel;
        private Label spiritClassLabel;
        private Label spiritLevelLabel;
        private Label spiritExpLabel;
        private Label rollsRemainingLabel;
        private Button rollButton;
        private Button setActiveButton;
        private Button removeButton;
        private ListView skillList;

        private uint playerGuid = 0;
        private int selectedSpiritIndex = -1;
        private List<SpiritData> spirits = new List<SpiritData>();

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            spiritList = root.Q<ListView>("spirit-list");
            spiritNameLabel = root.Q<Label>("spirit-name");
            spiritQualityLabel = root.Q<Label>("spirit-quality");
            spiritClassLabel = root.Q<Label>("spirit-class");
            spiritLevelLabel = root.Q<Label>("spirit-level");
            spiritExpLabel = root.Q<Label>("spirit-exp");
            rollsRemainingLabel = root.Q<Label>("rolls-remaining");
            rollButton = root.Q<Button>("roll-button");
            setActiveButton = root.Q<Button>("set-active-button");
            removeButton = root.Q<Button>("remove-button");
            skillList = root.Q<ListView>("skill-list");

            if (rollButton != null) rollButton.clicked += OnRollClicked;
            if (setActiveButton != null) setActiveButton.clicked += OnSetActiveClicked;
            if (removeButton != null) removeButton.clicked += OnRemoveClicked;

            RefreshUI();
        }

        private void OnDisable()
        {
            if (rollButton != null) rollButton.clicked -= OnRollClicked;
            if (setActiveButton != null) setActiveButton.clicked -= OnSetActiveClicked;
            if (removeButton != null) removeButton.clicked -= OnRemoveClicked;
        }

        public void SetPlayerInfo(uint guid)
        {
            playerGuid = guid;
            RefreshUI();
        }

        public void AddSpirit(SpiritData spirit)
        {
            spirits.Add(spirit);
            RefreshSpiritList();
        }

        public void RemoveSpirit(ulong spiritId)
        {
            spirits.RemoveAll(s => s.SpiritId == spiritId);
            RefreshSpiritList();
        }

        public void SetActiveSpirit(ulong spiritId)
        {
            foreach (var s in spirits) s.IsActive = (s.SpiritId == spiritId);
            RefreshSpiritList();
        }

        public void UpdateRollsRemaining(int remaining)
        {
            if (rollsRemainingLabel != null)
                rollsRemainingLabel.text = $"Круток осталось: {remaining}/10";
        }
        private void RefreshUI()
        {
            RefreshSpiritList();
            UpdateSpiritDetails();
            UpdateRollsRemaining(10);
        }

        private void RefreshSpiritList()
        {
            if (spiritList == null) return;

            var items = spirits.Select(s => $"{SpiritDataHelper.QualityNamesRu[(int)s.Quality]} {SpiritDataHelper.ClassNamesRu[(int)s.SpiritClass]} (Ур.{s.Level})").ToList();
            spiritList.itemsSource = items;
            spiritList.makeItem = () => new Label();
            spiritList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
            spiritList.selectionChanged += OnSpiritSelected;
        }

        private void OnSpiritSelected(IEnumerable<object> selectedItems)
        {
            selectedSpiritIndex = spiritList.selectedIndex;
            UpdateSpiritDetails();
        }

        private void UpdateSpiritDetails()
        {
            if (selectedSpiritIndex < 0 || selectedSpiritIndex >= spirits.Count) return;
            var spirit = spirits[selectedSpiritIndex];

            if (spiritNameLabel != null)
                spiritNameLabel.text = $"{SpiritDataHelper.QualityNamesRu[(int)spirit.Quality]} {SpiritDataHelper.ClassNamesRu[(int)spirit.SpiritClass]}";

            if (spiritQualityLabel != null)
                spiritQualityLabel.text = $"Качество: {SpiritDataHelper.QualityNamesRu[(int)spirit.Quality]}";

            if (spiritClassLabel != null)
                spiritClassLabel.text = $"Класс: {SpiritDataHelper.ClassNamesRu[(int)spirit.SpiritClass]}";

            if (spiritLevelLabel != null)
                spiritLevelLabel.text = $"Уровень: {spirit.Level}";

            if (spiritExpLabel != null)
                spiritExpLabel.text = $"Опыт: {spirit.Experience}/{SpiritDataHelper.GetExpForLevel(spirit.Level)}";

            if (skillList != null)
            {
                var skillItems = spirit.Skills.Select(sid =>
                {
                    var skill = SpiritDataHelper.Skills.FirstOrDefault(s => s.SkillId == sid);
                    int skillLevel = spirit.SkillLevels.ContainsKey(sid) ? spirit.SkillLevels[sid] : 1;
                    return skill != null ? $"{skill.NameRu} (Ур.{skillLevel})" : $"Skill {sid} (Ур.{skillLevel})";
                }).ToList();
                skillList.itemsSource = skillItems;
                skillList.makeItem = () => new Label();
                skillList.bindItem = (element, index) =>
                {
                    var label = (Label)element;
                    label.text = skillItems[index];
                };
            }
        }

        private void OnRollClicked()
        {
            Debug.Log("Rolling spirit...");
        }

        private void OnSetActiveClicked()
        {
            if (selectedSpiritIndex < 0 || selectedSpiritIndex >= spirits.Count) return;
            var spirit = spirits[selectedSpiritIndex];
            SetActiveSpirit(spirit.SpiritId);
            Debug.Log($"Setting active spirit: {spirit.SpiritId}");
        }

        private void OnRemoveClicked()
        {
            if (selectedSpiritIndex < 0 || selectedSpiritIndex >= spirits.Count) return;
            var spirit = spirits[selectedSpiritIndex];
            RemoveSpirit(spirit.SpiritId);
            Debug.Log($"Removing spirit: {spirit.SpiritId}");
        }
    }
}
