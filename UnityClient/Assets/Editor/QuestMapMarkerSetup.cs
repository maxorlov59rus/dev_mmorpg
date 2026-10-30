// QuestMapMarkerSetup.cs — Editor script to auto-create quest map markers
// Place in Assets/Editor/ folder
// Usage: Tools > Quest System > Create Quest Map Markers
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class QuestMapMarkerSetup : EditorWindow
    {
        [MenuItem("Tools/Quest System/Create Quest Map Markers")]
        public static void CreateQuestMapMarkers()
        {
            // Find or create parent
            GameObject parent = GameObject.Find("QuestMapMarkers");
            if (parent == null)
            {
                parent = new GameObject("QuestMapMarkers");
                Undo.RegisterCreatedObjectUndo(parent, "Create QuestMapMarkers");
            }

            // Quest NPC data: (npcId, questId, questName, worldPosition, markerType)
            var questNpcs = new (int npcId, int questId, string questName, Vector3 pos, QuestMapMarker.MarkerType type)[]
            {
                // Faction 1 (Covenant of Dawn) - Starter zone
                (90010, 1011, "Первые шаги", new Vector3(100, 0, 100), QuestMapMarker.MarkerType.QuestGiver),
                (90011, 1015, "Готовность к бою", new Vector3(150, 0, 120), QuestMapMarker.MarkerType.QuestGiver),
                (70011, 1001, "Первые испытания", new Vector3(200, 0, 150), QuestMapMarker.MarkerType.QuestGiver),
                (70011, 1009, "Зачистка аванпоста", new Vector3(300, 0, 200), QuestMapMarker.MarkerType.QuestGiver),
                (70011, 1013, "Охота на волков", new Vector3(120, 0, 80), QuestMapMarker.MarkerType.QuestGiver),
                (70012, 1004, "Путь избранного", new Vector3(250, 0, 180), QuestMapMarker.MarkerType.QuestGiver),
                (70012, 1005, "Проверка боем", new Vector3(250, 0, 180), QuestMapMarker.MarkerType.QuestGiver),
                (70013, 1015, "Травница просит", new Vector3(180, 0, 140), QuestMapMarker.MarkerType.QuestGiver),
                (70014, 1003, "Снабжение фронта", new Vector3(220, 0, 160), QuestMapMarker.MarkerType.QuestGiver),
                (70014, 1014, "Медь для кузнеца", new Vector3(220, 0, 160), QuestMapMarker.MarkerType.QuestGiver),
                (70015, 1010, "Сбитие гарпий", new Vector3(350, 0, 250), QuestMapMarker.MarkerType.QuestGiver),
                (70015, 1011, "Разведка Крепости Пепла", new Vector3(350, 0, 250), QuestMapMarker.MarkerType.QuestGiver),
                (70015, 1012, "Штурм крепости", new Vector3(350, 0, 250), QuestMapMarker.MarkerType.QuestGiver),

                // Faction 1 - Level 16-26
                (70021, 1016, "Тёмные предгорья", new Vector3(400, 0, 300), QuestMapMarker.MarkerType.QuestGiver),
                (70020, 1019, "Тайны древнего храма", new Vector3(500, 0, 350), QuestMapMarker.MarkerType.QuestGiver),
                (70022, 1021, "Охота на демонов", new Vector3(600, 0, 400), QuestMapMarker.MarkerType.QuestGiver),
                (70023, 1022, "Сбор артефактов", new Vector3(650, 0, 450), QuestMapMarker.MarkerType.QuestGiver),
                (70024, 1024, "Поверженный лорд", new Vector3(700, 0, 500), QuestMapMarker.MarkerType.QuestGiver),
                (70025, 1025, "Возвращение домой", new Vector3(750, 0, 550), QuestMapMarker.MarkerType.QuestGiver),
                (70025, 1026, "Финальное испытание", new Vector3(750, 0, 550), QuestMapMarker.MarkerType.QuestGiver),

                // Spirit Healers
                (80001, 0, "Духовный целитель Аэлия", new Vector3(125, 0, 50), QuestMapMarker.MarkerType.QuestGiver),
                (80002, 0, "Духовный целитель Торн", new Vector3(345, 0, 125), QuestMapMarker.MarkerType.QuestGiver),
                (80003, 0, "Духовный целитель Миранда", new Vector3(505, 0, 205), QuestMapMarker.MarkerType.QuestGiver),
            };

            int created = 0;
            foreach (var data in questNpcs)
            {
                // Check if marker already exists
                string markerName = $"QuestMarker_{data.npcId}_{data.questId}";
                if (parent.transform.Find(markerName) != null)
                    continue;

                GameObject marker = new GameObject(markerName);
                marker.transform.SetParent(parent.transform);
                marker.transform.position = data.pos;

                var uiDoc = marker.AddComponent<UIDocument>();
                var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>("Assets/UI/QuestMapMarker.uxml");
                if (uxml != null)
                    uiDoc.visualTreeAsset = uxml;

                var markerComp = marker.AddComponent<QuestMapMarker>();
                markerComp.SetQuest(data.questId, data.questName, data.type, data.pos);

                Undo.RegisterCreatedObjectUndo(marker, "Create Quest Map Marker");
                created++;
            }

            Debug.Log($"Created {created} quest map markers. Total: {questNpcs.Length}");
            Selection.activeGameObject = parent;
        }

        [MenuItem("Tools/Quest System/Verify Quest Map Markers")]
        public static void VerifyQuestMapMarkers()
        {
            var parent = GameObject.Find("QuestMapMarkers");
            if (parent == null)
            {
                Debug.LogError("QuestMapMarkers parent not found!");
                return;
            }

            int count = parent.transform.childCount;
            Debug.Log($"Found {count} quest map markers.");

            foreach (Transform child in parent.transform)
            {
                var marker = child.GetComponent<QuestMapMarker>();
                if (marker != null)
                {
                    Debug.Log($"  {child.name}: Quest {marker.questId} - {marker.questName} ({marker.markerType})");
                }
            }
        }
    }
}