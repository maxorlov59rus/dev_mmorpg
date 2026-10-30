// InstanceLoadingSetupEditor.cs — Editor script to auto-setup instance loading GameObjects
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class InstanceLoadingSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Instance Loading/Setup Loading GameObject")]
        public static void SetupLoadingGameObject()
        {
            GameObject parent = GameObject.Find("InstanceLoadingSystem");
            if (parent == null)
            {
                parent = new GameObject("InstanceLoadingSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create InstanceLoadingSystem");
            }

            GameObject loadingGo = new GameObject("InstanceLoading");
            loadingGo.transform.SetParent(parent.transform);
            loadingGo.AddComponent<UIDocument>();
            loadingGo.AddComponent<InstanceLoadingWidget>();
            Undo.RegisterCreatedObjectUndo(loadingGo, "Create InstanceLoading");

            var uiDoc = loadingGo.GetComponent<UIDocument>();
            string path = "Assets/UI/InstanceLoading.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("InstanceLoading.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            Selection.activeGameObject = loadingGo;
            Debug.Log("Instance Loading GameObject created and configured");
        }

        [MenuItem("Tools/Instance Loading/Verify Loading Setup")]
        public static void VerifyLoadingSetup()
        {
            var go = GameObject.Find("InstanceLoading");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: InstanceLoading");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<InstanceLoadingWidget>();
            if (script != null)
                Debug.Log("  ✓ InstanceLoadingWidget attached");
            else
                Debug.LogError("  ✗ InstanceLoadingWidget missing");
        }
    }
}
