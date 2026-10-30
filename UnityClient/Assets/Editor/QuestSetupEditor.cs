// QuestSetupEditor.cs — Editor script to auto-setup all quest system GameObjects
// Place in Assets/Editor/ folder
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using UnityEditor.UIElements;
using DM.Client;
using DarkMatter.UI;
using DarkMatter.Network;

namespace DM.Editor
{
    public class QuestSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Quest System/Setup All GameObjects")]
        public static void SetupAllGameObjects()
        {
            // Create parent
            GameObject parent = new GameObject("QuestSystem");
            Undo.RegisterCreatedObjectUndo(parent, "Create QuestSystem");

            // 1. TimeDisplay
            CreateGameObjectWithComponents(parent, "TimeDisplay", 
                typeof(UIDocument), typeof(TimeDisplay));
            AssignUxml(parent.transform.Find("TimeDisplay").gameObject, "TimeDisplay");

            // 2. WeatherIndicator
            CreateGameObjectWithComponents(parent, "WeatherIndicator",
                typeof(UIDocument), typeof(WeatherIndicator));
            AssignUxml(parent.transform.Find("WeatherIndicator").gameObject, "WeatherIndicator");

            // 3. CharacterCreation
            CreateGameObjectWithComponents(parent, "CharacterCreation",
                typeof(UIDocument), typeof(CharacterCreationWidget));
            AssignUxml(parent.transform.Find("CharacterCreation").gameObject, "CharacterCreation");

            // 4. QuestLog
            CreateGameObjectWithComponents(parent, "QuestLog",
                typeof(UIDocument), typeof(QuestLogWidget));
            AssignUxml(parent.transform.Find("QuestLog").gameObject, "QuestLog");

            // 5. QuestDialogue
            CreateGameObjectWithComponents(parent, "QuestDialogue",
                typeof(UIDocument), typeof(QuestDialogueUI));
            AssignUxml(parent.transform.Find("QuestDialogue").gameObject, "QuestDialogue");

            // 6. Resurrection
            CreateGameObjectWithComponents(parent, "Resurrection",
                typeof(UIDocument), typeof(ResurrectionClient));
            AssignUxml(parent.transform.Find("Resurrection").gameObject, "Resurrection");

            // 7. NetworkWidgetBinder
            CreateGameObjectWithComponents(parent, "NetworkWidgetBinder",
                typeof(NetworkWidgetBinder));

            // Configure NetworkWidgetBinder references
            ConfigureNetworkWidgetBinder(parent);

            // 8. MistyAnomaly
            CreateGameObjectWithComponents(parent, "MistyAnomaly",
                typeof(UIDocument), typeof(MistyAnomalyClient));

            // 9. SeasonalEvent
            CreateGameObjectWithComponents(parent, "SeasonalEvent",
                typeof(UIDocument), typeof(SeasonalEventClient));
            AssignUxml(parent.transform.Find("SeasonalEvent").gameObject, "SeasonalEvent");
            AssignUxml(parent.transform.Find("MistyAnomaly").gameObject, "MistyAnomaly");
            Selection.activeGameObject = parent;
        }

        static void CreateGameObjectWithComponents(GameObject parent, string name, params System.Type[] components)
        {
            GameObject go = new GameObject(name);
            go.transform.SetParent(parent.transform);
            foreach (var comp in components)
            {
                go.AddComponent(comp);
            }
            Undo.RegisterCreatedObjectUndo(go, "Create " + name);
        }

        static void AssignUxml(GameObject go, string uxmlName)
        {
            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null)
            {
                string path = $"Assets/UI/{uxmlName}.uxml";
                var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
                if (uxml != null)
                {
                    uiDoc.visualTreeAsset = uxml;
                    EditorUtility.SetDirty(uiDoc);
                }
                else
                {
                    Debug.LogWarning($"UXML not found: {path}");
                }
            }
        }

        static void ConfigureNetworkWidgetBinder(GameObject parent)
        {
            var binder = parent.transform.Find("NetworkWidgetBinder")?.GetComponent<NetworkWidgetBinder>();
            if (binder == null) return;

            // Find MMONetworkClient (or create reference)
            var networkClient = Object.FindAnyObjectByType<MMONetworkClient>();
            if (networkClient != null)
                binder.networkClient = networkClient;

            // Assign widget references
            binder.timeDisplay = parent.transform.Find("TimeDisplay")?.GetComponent<TimeDisplay>();
            binder.weatherIndicator = parent.transform.Find("WeatherIndicator")?.GetComponent<WeatherIndicator>();
            binder.characterCreationWidget = parent.transform.Find("CharacterCreation")?.GetComponent<CharacterCreationWidget>();
            binder.questLogWidget = parent.transform.Find("QuestLog")?.GetComponent<QuestLogWidget>();

            EditorUtility.SetDirty(binder);
        }

        [MenuItem("Tools/Quest System/Verify Setup")]
        public static void VerifySetup()
        {
            string[] requiredObjects = { "TimeDisplay", "WeatherIndicator", "CharacterCreation", 
                                         "QuestLog", "QuestDialogue", "Resurrection", "NetworkWidgetBinder",
                                         "MistyAnomaly" };

            foreach (string objName in requiredObjects)
            {
                var go = GameObject.Find(objName);
                if (go == null)
                {
                    Debug.LogError($"Missing GameObject: {objName}");
                }
                else
                {
                    Debug.Log($"Found: {objName}");
                    
                    // Check UIDocument
                    var uiDoc = go.GetComponent<UIDocument>();
                    if (uiDoc != null && uiDoc.visualTreeAsset != null)
                        Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
                    else if (uiDoc != null)
                        Debug.LogWarning($"  ⚠ UIDocument has no UXML assigned");
                    
                    // Check script
                    var script = go.GetComponent<MonoBehaviour>();
                    if (script != null)
                        Debug.Log($"  ✓ Script attached: {script.GetType().Name}");
                }
            }
        }
    }
}