// QuestMapMarker.cs — Маркеры квестов на внутриигровой карте
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    /// <summary>
    /// Displays quest markers on the in-game map.
    /// </summary>
    public class QuestMapMarker : MonoBehaviour
    {
        [Header("Marker Settings")]
        public int questId = 0;
        public string questName = "";
        public Vector3 worldPosition;
        public MarkerType markerType = MarkerType.QuestGiver;

        public enum MarkerType
        {
            QuestGiver,     // NPC даёт квест
            QuestObjective,  // Цель квеста (зона)
            QuestTurnIn,    // NPC для сдачи квеста
            QuestComplete   // Квест завершён
        }

        [Header("UI")]
        private VisualElement _markerElement;
        private Label _markerLabel;
        private UIDocument _uiDocument;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _markerElement = root.Q<VisualElement>("QuestMarker");
                _markerLabel = root.Q<Label>("MarkerLabel");

                UpdateMarkerAppearance();
            }
        }

        void Update()
        {
            // Update marker position on map
            if (_markerElement != null)
            {
                // Convert world position to map position
                Vector2 mapPos = WorldToMapPosition(worldPosition);
                _markerElement.style.left = mapPos.x;
                _markerElement.style.top = mapPos.y;
            }
        }

        void UpdateMarkerAppearance()
        {
            if (_markerElement == null) return;

            // Remove all marker classes
            _markerElement.RemoveFromClassList("marker-quest-giver");
            _markerElement.RemoveFromClassList("marker-objective");
            _markerElement.RemoveFromClassList("marker-turn-in");
            _markerElement.RemoveFromClassList("marker-complete");

            // Add appropriate class
            switch (markerType)
            {
                case MarkerType.QuestGiver:
                    _markerElement.AddToClassList("marker-quest-giver");
                    if (_markerLabel != null)
                        _markerLabel.text = "!";
                    break;
                case MarkerType.QuestObjective:
                    _markerElement.AddToClassList("marker-objective");
                    if (_markerLabel != null)
                        _markerLabel.text = "?";
                    break;
                case MarkerType.QuestTurnIn:
                    _markerElement.AddToClassList("marker-turn-in");
                    if (_markerLabel != null)
                        _markerLabel.text = "?";
                    break;
                case MarkerType.QuestComplete:
                    _markerElement.AddToClassList("marker-complete");
                    if (_markerLabel != null)
                        _markerLabel.text = "✓";
                    break;
            }

            if (_markerLabel != null && !string.IsNullOrEmpty(questName))
                _markerLabel.tooltip = questName;
        }

        Vector2 WorldToMapPosition(Vector3 worldPos)
        {
            // Simple conversion - adjust based on your map
            // Assuming map covers 0-1000 in both X and Z
            float mapX = (worldPos.x / 1000f) * 100f; // percentage
            float mapY = (worldPos.z / 1000f) * 100f; // percentage
            return new Vector2(mapX, mapY);
        }

        public void SetQuest(int questId, string questName, MarkerType type, Vector3 worldPos)
        {
            this.questId = questId;
            this.questName = questName;
            this.markerType = type;
            this.worldPosition = worldPos;
            UpdateMarkerAppearance();
        }
    }
}