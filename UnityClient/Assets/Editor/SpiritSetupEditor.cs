// SpiritSetupEditor.cs — Editor script to auto-setup spirit system GameObjects
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class SpiritSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Spirit System/Setup Spirit GameObject")]
        public static void SetupSpiritGameObject()
        {
            GameObject parent = GameObject.Find("SpiritSystem");
            if (parent == null)
            {
                parent = new GameObject("SpiritSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create SpiritSystem");
            }

            GameObject spiritGo = new GameObject("Spirit");
            spiritGo.transform.SetParent(parent.transform);
            spiritGo.AddComponent<UIDocument>();
            spiritGo.AddComponent<SpiritWidget>();
            Undo.RegisterCreatedObjectUndo(spiritGo, "Create Spirit");

            var uiDoc = spiritGo.GetComponent<UIDocument>();
            string path = "Assets/UI/Spirit.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("Spirit.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            var binder = Object.FindAnyObjectByType<NetworkWidgetBinder>();
            if (binder != null)
            {
                EditorUtility.SetDirty(binder);
            }

            Selection.activeGameObject = spiritGo;
            Debug.Log("Spirit GameObject created and configured");
        }

        [MenuItem("Tools/Spirit System/Verify Spirit Setup")]
        public static void VerifySpiritSetup()
        {
            var go = GameObject.Find("Spirit");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: Spirit");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<SpiritWidget>();
            if (script != null)
                Debug.Log("  ✓ SpiritWidget attached");
            else
                Debug.LogError("  ✗ SpiritWidget missing");
        }
    }
}
