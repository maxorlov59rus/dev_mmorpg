using UnityEngine;
using UnityEngine.UIElements;
using System;
using System.Collections;
using System.Collections.Generic;

namespace DM.Client
{
    public class InstanceLoadingWidget : MonoBehaviour
    {
        [SerializeField] private UIDocument uiDocument;

        private VisualElement root;
        private VisualElement background;
        private Label instanceNameLabel;
        private Label tipLabel;
        private Label progressLabel;
        private ProgressBar progressBar;
        private VisualElement fadeOverlay;

        private LoadingScreenData currentScreen;
        private bool isLoading = false;
        private float loadingProgress = 0f;
        private float loadingStartTime = 0f;
        private float loadingDuration = 3f;

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            root = uiDocument.rootVisualElement;

            background = root.Q<VisualElement>("background");
            instanceNameLabel = root.Q<Label>("instance-name");
            tipLabel = root.Q<Label>("tip-label");
            progressLabel = root.Q<Label>("progress-label");
            progressBar = root.Q<ProgressBar>("progress-bar");
            fadeOverlay = root.Q<VisualElement>("fade-overlay");

            // Hide by default
            if (root != null) root.style.display = DisplayStyle.None;
        }

        public void StartLoading(int mapId)
        {
            currentScreen = LoadingScreenHelper.GetScreenForMap(mapId);
            if (currentScreen == null) return;

            isLoading = true;
            loadingProgress = 0f;
            loadingDuration = currentScreen.AnimationDurationMs / 1000f;
            loadingStartTime = Time.time;

            if (root != null) root.style.display = DisplayStyle.Flex;

            // Set background
            if (background != null)
            {
                // TODO: Load background image from Resources
                background.style.backgroundImage = null;
            }

            // Set text
            if (instanceNameLabel != null)
                instanceNameLabel.text = currentScreen.NameRu;

            if (tipLabel != null)
                tipLabel.text = currentScreen.TipRu;

            // Play animation
            StartCoroutine(PlayLoadingAnimation());
        }

        public void StopLoading()
        {
            isLoading = false;
            if (root != null) root.style.display = DisplayStyle.None;
        }

        public void UpdateProgress(float progress)
        {
            loadingProgress = Mathf.Clamp01(progress);
            if (progressBar != null)
                progressBar.value = loadingProgress * 100f;
            if (progressLabel != null)
                progressLabel.text = $"{(int)(loadingProgress * 100)}%";
        }

        private IEnumerator PlayLoadingAnimation()
        {
            // Fade in
            if (fadeOverlay != null)
            {
                fadeOverlay.style.opacity = 1f;
                float fadeInTime = 0.5f;
                float elapsed = 0f;
                while (elapsed < fadeInTime)
                {
                    elapsed += Time.deltaTime;
                    fadeOverlay.style.opacity = 1f - (elapsed / fadeInTime);
                    yield return null;
                }
                fadeOverlay.style.opacity = 0f;
            }

            // Loading progress
            while (loadingProgress < 1f && isLoading)
            {
                float elapsed = Time.time - loadingStartTime;
                loadingProgress = Mathf.Clamp01(elapsed / loadingDuration);
                UpdateProgress(loadingProgress);
                yield return null;
            }

            // Ensure 100%
            UpdateProgress(1f);

            // Wait a bit
            yield return new WaitForSeconds(0.5f);

            // Fade out
            if (fadeOverlay != null)
            {
                fadeOverlay.style.opacity = 0f;
                float fadeOutTime = 0.5f;
                float elapsed = 0f;
                while (elapsed < fadeOutTime)
                {
                    elapsed += Time.deltaTime;
                    fadeOverlay.style.opacity = elapsed / fadeOutTime;
                    yield return null;
                }
                fadeOverlay.style.opacity = 1f;
            }

            // Hide
            StopLoading();
        }
    }
}
