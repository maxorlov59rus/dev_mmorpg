// BeginnerBuffSetupEditor.cs — Editor script to auto-setup beginner buff GameObject
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class BeginnerBuffSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Beginner Buff/Setup Beginner Buff GameObject")]
        public static void SetupBeginnerBuffGameObject()
        {
            GameObject parent = GameObject.Find("BeginnerBuffSystem");
            if (parent == null)
            {
                parent = new GameObject("BeginnerBuffSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create BeginnerBuffSystem");
            }

            GameObject buffGo = new GameObject("BeginnerBuff");
            buffGo.transform.SetParent(parent.transform);
            buffGo.AddComponent<UIDocument>();
            buffGo.AddComponent<BeginnerBuffWidget>();
            Undo.RegisterCreatedObjectUndo(buffGo, "Create BeginnerBuff");

            var uiDoc = buffGo.GetComponent<UIDocument>();
            string path = "Assets/UI/BeginnerBuff.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("BeginnerBuff.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            Selection.activeGameObject = buffGo;
            Debug.Log("Beginner Buff GameObject created and configured");
        }

        [MenuItem("Tools/Beginner Buff/Verify Beginner Buff Setup")]
        public static void VerifyBeginnerBuffSetup()
        {
            var go = GameObject.Find("BeginnerBuff");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: BeginnerBuff");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<BeginnerBuffWidget>();
            if (script != null)
                Debug.Log("  ✓ BeginnerBuffWidget attached");
            else
                Debug.LogError("  ✗ BeginnerBuffWidget missing");
        }
    }
}
