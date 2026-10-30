// SpiritBattleSetupEditor.cs — Editor script to auto-setup Spirit Battle GameObjects
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class SpiritBattleSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Spirit Battle/Setup Spirit Battle GameObject")]
        public static void SetupSpiritBattleGameObject()
        {
            GameObject parent = GameObject.Find("SpiritBattleSystem");
            if (parent == null)
            {
                parent = new GameObject("SpiritBattleSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create SpiritBattleSystem");
            }

            GameObject battleGo = new GameObject("SpiritBattle");
            battleGo.transform.SetParent(parent.transform);
            battleGo.AddComponent<UIDocument>();
            battleGo.AddComponent<SpiritBattleWidget>();
            Undo.RegisterCreatedObjectUndo(battleGo, "Create SpiritBattle");

            var uiDoc = battleGo.GetComponent<UIDocument>();
            string path = "Assets/UI/SpiritBattle.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("SpiritBattle.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            Selection.activeGameObject = battleGo;
            Debug.Log("Spirit Battle GameObject created and configured");
        }

        [MenuItem("Tools/Spirit Battle/Verify Spirit Battle Setup")]
        public static void VerifySpiritBattleSetup()
        {
            var go = GameObject.Find("SpiritBattle");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: SpiritBattle");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<SpiritBattleWidget>();
            if (script != null)
                Debug.Log("  ✓ SpiritBattleWidget attached");
            else
                Debug.LogError("  ✗ SpiritBattleWidget missing");
        }
    }
}
