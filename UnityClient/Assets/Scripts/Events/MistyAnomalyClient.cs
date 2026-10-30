// MistyAnomalyClient.cs — Клиентская система эвента "Туманная Аномалия" (UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;
using System.Collections.Generic;

namespace DM.Client
{
    public class MistyAnomalyClient : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _root;
        private Label _eventName;
        private Label _timer;
        private Label _playerScore;
        private Label _leaderboard;
        private Label _fogDensity;
        private Button _joinButton;
        private Button _leaveButton;
        private Button _closeButton;

        private bool _isInEvent = false;
        private float _eventTimer = 0f;
        private int _playerScoreValue = 0;
        private List<PlayerScore> _leaderboardData = new List<PlayerScore>();

        [System.Serializable]
        public class PlayerScore
        {
            public string playerName;
            public int score;
            public int kills;
            public int deaths;
        }

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                _root = _uiDocument.rootVisualElement;
                _eventName = _root.Q<Label>("EventName");
                _timer = _root.Q<Label>("EventTimer");
                _playerScore = _root.Q<Label>("PlayerScore");
                _leaderboard = _root.Q<Label>("Leaderboard");
                _fogDensity = _root.Q<Label>("FogDensity");
                _joinButton = _root.Q<Button>("JoinButton");
                _leaveButton = _root.Q<Button>("LeaveButton");
                _closeButton = _root.Q<Button>("CloseButton");

                if (_joinButton != null)
                    _joinButton.clicked += OnJoinClicked;
                if (_leaveButton != null)
                    _leaveButton.clicked += OnLeaveClicked;
                if (_closeButton != null)
                    _closeButton.clicked += OnCloseClicked;
                HideEventUI();
            }
        }

        void OnDisable()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_leaveButton != null)
                _leaveButton.clicked -= OnLeaveClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }

        void Update()
        {
            if (_isInEvent)
            {
                _eventTimer -= Time.deltaTime;
                if (_eventTimer <= 0)
                {
                    _eventTimer = 0;
                    EndEvent();
                }

                if (_timer != null)
                    _timer.text = $"Время: {Mathf.CeilToInt(_eventTimer)} сек";

                if (_playerScore != null)
                    _playerScore.text = $"Ваши очки: {_playerScoreValue}";
            }
        }

        public void OnEventStart(float durationSec)
        {
            _isInEvent = true;
            _eventTimer = durationSec;
            _playerScoreValue = 0;

            if (_joinButton != null)
                _joinButton.style.display = DisplayStyle.None;
            if (_leaveButton != null)
                _leaveButton.style.display = DisplayStyle.Flex;

            Debug.Log("Misty Anomaly event started!");
        }

        public void OnEventEnd()
        {
            _isInEvent = false;

            if (_joinButton != null)
                _joinButton.style.display = DisplayStyle.Flex;
            if (_leaveButton != null)
                _leaveButton.style.display = DisplayStyle.None;

            Debug.Log($"Misty Anomaly event ended! Final score: {_playerScoreValue}");
        }

        void EndEvent()
        {
            OnEventEnd();
        }

        public void OnMobKill(int score)
        {
            _playerScoreValue += score;
            if (_playerScore != null)
                _playerScore.text = $"Ваши очки: {_playerScoreValue}";
        }

        public void OnPlayerKill(int score)
        {
            _playerScoreValue += score;
            if (_playerScore != null)
                _playerScore.text = $"Ваши очки: {_playerScoreValue}";
        }

        public void OnBossKill(int score)
        {
            _playerScoreValue += score;
            if (_playerScore != null)
                _playerScore.text = $"Ваши очки: {_playerScoreValue}";
        }

        public void UpdateLeaderboard(List<PlayerScore> scores)
        {
            _leaderboardData = scores;
            if (_leaderboard != null)
            {
                string text = "Таблица лидеров:\n";
                int rank = 1;
                foreach (var score in scores)
                {
                    text += $"{rank}. {score.playerName} — {score.score} очков\n";
                    rank++;
                }
                _leaderboard.text = text;
            }
        }

        void OnJoinClicked()
        {
            Debug.Log("Joining Misty Anomaly event...");
        }

        void OnLeaveClicked()
        {
            Debug.Log("Leaving Misty Anomaly event...");
            _isInEvent = false;
            HideEventUI();
        }

        void OnCloseClicked()
        {
            gameObject.SetActive(false);
        }

        void HideEventUI()
        {
            if (_joinButton != null)
                _joinButton.style.display = DisplayStyle.Flex;
            if (_leaveButton != null)
                _leaveButton.style.display = DisplayStyle.None;
        }

        void OnDestroy()
        {
            if (_joinButton != null)
                _joinButton.clicked -= OnJoinClicked;
            if (_leaveButton != null)
                _leaveButton.clicked -= OnLeaveClicked;
            if (_closeButton != null)
                _closeButton.clicked -= OnCloseClicked;
        }
    }
}
