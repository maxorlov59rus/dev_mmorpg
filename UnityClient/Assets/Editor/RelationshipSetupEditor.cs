// RelationshipSetupEditor.cs — Editor script to auto-setup relationship system GameObjects
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class RelationshipSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Relationship System/Setup Relationship GameObject")]
        public static void SetupRelationshipGameObject()
        {
            GameObject parent = GameObject.Find("RelationshipSystem");
            if (parent == null)
            {
                parent = new GameObject("RelationshipSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create RelationshipSystem");
            }

            GameObject relGo = new GameObject("Relationship");
            relGo.transform.SetParent(parent.transform);
            relGo.AddComponent<UIDocument>();
            relGo.AddComponent<RelationshipWidget>();
            Undo.RegisterCreatedObjectUndo(relGo, "Create Relationship");

            var uiDoc = relGo.GetComponent<UIDocument>();
            string path = "Assets/UI/Relationship.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("Relationship.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            var binder = Object.FindAnyObjectByType<NetworkWidgetBinder>();
            if (binder != null)
            {
                // binder.relationshipWidget = relGo.GetComponent<RelationshipWidget>();
                EditorUtility.SetDirty(binder);
            }

            Selection.activeGameObject = relGo;
            Debug.Log("Relationship GameObject created and configured");
        }

        [MenuItem("Tools/Relationship System/Verify Relationship Setup")]
        public static void VerifyRelationshipSetup()
        {
            var go = GameObject.Find("Relationship");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: Relationship");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<RelationshipWidget>();
            if (script != null)
                Debug.Log("  ✓ RelationshipWidget attached");
            else
                Debug.LogError("  ✗ RelationshipWidget missing");
        }
    }
}
