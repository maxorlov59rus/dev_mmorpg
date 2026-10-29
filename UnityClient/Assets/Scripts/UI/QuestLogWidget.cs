// QuestLogWidget.cs — заглушка UI журнала квестов
using UnityEngine;
using UnityEngine.UIElements;

namespace DarkMatter.UI
{
    public class QuestLogWidget : MonoBehaviour
    {
        public VisualTreeAsset Uxml;
        private VisualElement root;

        void OnEnable()
        {
            root = GetComponent<UIDocument>().rootVisualElement;
            // TODO: реализовать список квестов
        }
    }
}
