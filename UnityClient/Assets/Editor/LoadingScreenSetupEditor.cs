// LoadingScreenSetupEditor.cs — Editor script to auto-setup loading screen GameObject
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class LoadingScreenSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Loading Screen/Setup Loading Screen GameObject")]
        public static void SetupLoadingScreenGameObject()
        {
            GameObject loadingGo = new GameObject("LoadingScreen");
            loadingGo.AddComponent<UIDocument>();
            loadingGo.AddComponent<LoadingScreenWidget>();
            Undo.RegisterCreatedObjectUndo(loadingGo, "Create LoadingScreen");

            var uiDoc = loadingGo.GetComponent<UIDocument>();
            string path = "Assets/UI/LoadingScreen.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("LoadingScreen.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            Selection.activeGameObject = loadingGo;
            Debug.Log("Loading Screen GameObject created and configured");
        }

        [MenuItem("Tools/Loading Screen/Verify Loading Screen Setup")]
        public static void VerifyLoadingScreenSetup()
        {
            var go = GameObject.Find("LoadingScreen");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: LoadingScreen");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<LoadingScreenWidget>();
            if (script != null)
                Debug.Log("  ✓ LoadingScreenWidget attached");
            else
                Debug.LogError("  ✗ LoadingScreenWidget missing");
        }
    }
}
