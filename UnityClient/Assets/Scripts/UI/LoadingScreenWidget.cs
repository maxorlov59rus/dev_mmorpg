using UnityEngine;
using UnityEngine.UIElements;
using System;
using System.Collections;

namespace DM.Client
{
    public class LoadingScreenWidget : MonoBehaviour
    {
        [SerializeField] private UIDocument uiDocument;

        private VisualElement root;
        private VisualElement background;
        private VisualElement logo;
        private Label titleLabel;
        private Label versionLabel;
        private Label statusLabel;
        private ProgressBar progressBar;
        private Button googleAuthButton;
        private Button playButton;
        private VisualElement authPanel;

        private bool isAuthenticating = false;
        private bool isAuthenticated = false;

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            root = uiDocument.rootVisualElement;

            background = root.Q<VisualElement>("background");
            logo = root.Q<VisualElement>("logo");
            titleLabel = root.Q<Label>("title-label");
            versionLabel = root.Q<Label>("version-label");
            statusLabel = root.Q<Label>("status-label");
            progressBar = root.Q<ProgressBar>("progress-bar");
            googleAuthButton = root.Q<Button>("google-auth-button");
            playButton = root.Q<Button>("play-button");
            authPanel = root.Q<VisualElement>("auth-panel");

            if (googleAuthButton != null) googleAuthButton.clicked += OnGoogleAuthClicked;
            if (playButton != null) playButton.clicked += OnPlayClicked;

            // Show loading screen
            if (root != null) root.style.display = DisplayStyle.Flex;

            // Start loading sequence
            StartCoroutine(LoadingSequence());
        }

        private void OnDisable()
        {
            if (googleAuthButton != null) googleAuthButton.clicked -= OnGoogleAuthClicked;
            if (playButton != null) playButton.clicked -= OnPlayClicked;
        }

        private IEnumerator LoadingSequence()
        {
            // Phase 1: Show logo and title
            if (titleLabel != null) titleLabel.text = "Чёрная Материя";
            if (versionLabel != null) versionLabel.text = "v1.0.0";
            if (statusLabel != null) statusLabel.text = "Загрузка...";

            // Simulate loading
            float progress = 0f;
            while (progress < 1f)
            {
                progress += Time.deltaTime * 0.5f;
                if (progressBar != null) progressBar.value = Mathf.Clamp01(progress) * 100f;
                yield return null;
            }

            // Phase 2: Show auth panel
            if (statusLabel != null) statusLabel.text = "Авторизация";
            if (authPanel != null) authPanel.style.display = DisplayStyle.Flex;
            if (playButton != null) playButton.style.display = DisplayStyle.None;
        }

        private void OnGoogleAuthClicked()
        {
            if (isAuthenticating) return;
            isAuthenticating = true;

            if (statusLabel != null) statusLabel.text = "Авторизация через Google...";
            if (googleAuthButton != null) googleAuthButton.SetEnabled(false);

            // TODO: Open Google OAuth URL in browser
            // Application.OpenURL(GoogleAuthMgr.GetAuthUrl());
            // Or use Unity's web view

            // Simulate auth for now
            StartCoroutine(SimulateGoogleAuth());
        }

        private IEnumerator SimulateGoogleAuth()
        {
            yield return new WaitForSeconds(2f);

            isAuthenticated = true;
            isAuthenticating = false;

            if (statusLabel != null) statusLabel.text = "Авторизация успешна!";
            if (googleAuthButton != null) googleAuthButton.style.display = DisplayStyle.None;
            if (playButton != null) playButton.style.display = DisplayStyle.Flex;
        }

        private void OnPlayClicked()
        {
            if (!isAuthenticated)
            {
                if (statusLabel != null) statusLabel.text = "Сначала авторизуйтесь!";
                return;
            }

            // Hide loading screen and start game
            if (root != null) root.style.display = DisplayStyle.None;
            Debug.Log("Starting game...");
            // TODO: Load main scene
        }

        public void SetStatus(string status)
        {
            if (statusLabel != null) statusLabel.text = status;
        }

        public void UpdateProgress(float progress)
        {
            if (progressBar != null) progressBar.value = progress * 100f;
        }
    }
}
