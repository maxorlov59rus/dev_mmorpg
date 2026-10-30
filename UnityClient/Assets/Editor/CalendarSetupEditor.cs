// CalendarSetupEditor.cs — Editor script to auto-setup calendar GameObject
using UnityEngine;
using UnityEngine.UIElements;
using UnityEditor;
using DM.Client;

namespace DM.Editor
{
    public class CalendarSetupEditor : EditorWindow
    {
        [MenuItem("Tools/Calendar/Setup Calendar GameObject")]
        public static void SetupCalendarGameObject()
        {
            GameObject parent = GameObject.Find("CalendarSystem");
            if (parent == null)
            {
                parent = new GameObject("CalendarSystem");
                Undo.RegisterCreatedObjectUndo(parent, "Create CalendarSystem");
            }

            GameObject calendarGo = new GameObject("Calendar");
            calendarGo.transform.SetParent(parent.transform);
            calendarGo.AddComponent<UIDocument>();
            calendarGo.AddComponent<CalendarWidget>();
            Undo.RegisterCreatedObjectUndo(calendarGo, "Create Calendar");

            var uiDoc = calendarGo.GetComponent<UIDocument>();
            string path = "Assets/UI/Calendar.uxml";
            var uxml = AssetDatabase.LoadAssetAtPath<VisualTreeAsset>(path);
            if (uxml != null)
            {
                uiDoc.visualTreeAsset = uxml;
                EditorUtility.SetDirty(uiDoc);
                Debug.Log("Calendar.uxml assigned successfully");
            }
            else
            {
                Debug.LogWarning($"UXML not found: {path}");
            }

            Selection.activeGameObject = calendarGo;
            Debug.Log("Calendar GameObject created and configured");
        }

        [MenuItem("Tools/Calendar/Verify Calendar Setup")]
        public static void VerifyCalendarSetup()
        {
            var go = GameObject.Find("Calendar");
            if (go == null)
            {
                Debug.LogError("Missing GameObject: Calendar");
                return;
            }

            var uiDoc = go.GetComponent<UIDocument>();
            if (uiDoc != null && uiDoc.visualTreeAsset != null)
                Debug.Log($"  ✓ UIDocument assigned: {uiDoc.visualTreeAsset.name}");
            else if (uiDoc != null)
                Debug.LogWarning("  ⚠ UIDocument has no UXML assigned");

            var script = go.GetComponent<CalendarWidget>();
            if (script != null)
                Debug.Log("  ✓ CalendarWidget attached");
            else
                Debug.LogError("  ✗ CalendarWidget missing");
        }
    }
}
