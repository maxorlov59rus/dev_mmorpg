// CraftSetupEditor.cs — Editor script to auto-setup crafting system GameObjects
// Place in Assets/Editor/ folder
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class CraftSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Craft System/Setup Craft GameObject")]
        public static void SetupCraftGameObject()
        {
            // Create parent if not exists
            GameObject parent = GameObject.Find("CraftSystem");
            if (parent == null)
            {
                parent = new GameObject("CraftSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create CraftSystem");
            }

            // Create Craft GameObject
            GameObject craftGo = new GameObject("Craft");
            craftGo.transform.SetParent(parent.transform);
            craftGo.AddComponent<UIDocument>();
            craftGo.AddComponent<CraftWidget>();
            Undo.RegisterCreatedObjectUndo(craftGo, "Create Craft");

            // Assign UXML
            var uiDoc = craftGo.GetComponent<UIDocument>();
            string path = "Assets/UI/Craft.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("Craft.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            // Configure NetworkWidgetBinder reference
            var binder = Object.FindAnyObjectByType<NetworkWidgetBinder>();
            if (binder != null)
            {
                binder.craftWidget = craftGo.GetComponent<CraftWidget>();
                EditorUtility.SetDirty(binder);
            }

            Selection.activeGameObject = craftGo;
            Debug.Log("Craft GameObject created and configured");
        }

        [MenuItem("Tools/Craft System/Verify Craft Setup")]
        public static void VerifyCraftSetup()
        {
            var go = GameObject.Find("Craft");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: Craft");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<CraftWidget>();
            if (script != null)
                Debug.Log($"  ✓ CraftWidget attached");
            else
                Debug.LogError("  ✗ CraftWidget missing");
        }
    }
}
