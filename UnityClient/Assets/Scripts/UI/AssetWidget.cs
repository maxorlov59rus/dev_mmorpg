using UnityEngine;
using UnityEngine.UIElements;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class AssetWidget : MonoBehaviour
    {
        [SerializeField] private UIDocument uiDocument;

        private ListView musicList;
        private ListView loreList;
        private Label selectedMusicLabel;
        private Label selectedLoreLabel;
        private Button playMusicButton;

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            musicList = root.Q<ListView>("music-list");
            loreList = root.Q<ListView>("lore-list");
            selectedMusicLabel = root.Q<Label>("selected-music");
            selectedLoreLabel = root.Q<Label>("selected-lore");
            playMusicButton = root.Q<Button>("play-music-button");

            if (playMusicButton != null) playMusicButton.clicked += OnPlayMusicClicked;

            RefreshMusicList();
            RefreshLoreList();
        }

        private void RefreshMusicList()
        {
            if (musicList == null) return;
            var items = AssetDataHelper.MusicTracks.Select(m => m.NameRu).ToList();
            musicList.itemsSource = items;
            musicList.makeItem = () => new Label();
            musicList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
        }

        private void RefreshLoreList()
        {
            if (loreList == null) return;
            var items = AssetDataHelper.LoreEntries.Select(l => l.TitleRu).ToList();
            loreList.itemsSource = items;
            loreList.makeItem = () => new Label();
            loreList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = items[index];
            };
        }

        private void OnPlayMusicClicked()
        {
            Debug.Log("Playing selected music track");
            // TODO: Play music via AudioSource
        }
    }
}
