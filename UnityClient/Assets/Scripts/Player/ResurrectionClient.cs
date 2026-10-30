// ResurrectionClient.cs — Клиентская система воскрешения (Unity UI Toolkit)
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    public class ResurrectionClient : MonoBehaviour
    {
        private UIDocument _uiDocument;
        private VisualElement _deathScreen;
        private VisualElement _ghostScreen;
        private Label _deathTimer;
        private Button _resurrectButton;
        private Button _releaseSpiritButton;
        private Button _returnToBodyButton;
        private Label _resurrectInfo;

        public float resurrectWindowSeconds = 60f;

        private bool _isDead = false;
        private bool _isGhost = false;
        private float _deathTimerValue = 0f;
        private Vector3 _corpsePosition;

        void OnEnable()
        {
            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _deathScreen = root.Q<VisualElement>("DeathScreen");
                _ghostScreen = root.Q<VisualElement>("GhostScreen");
                _deathTimer = root.Q<Label>("DeathTimer");
                _resurrectButton = root.Q<Button>("ResurrectButton");
                _releaseSpiritButton = root.Q<Button>("ReleaseSpiritButton");
                _returnToBodyButton = root.Q<Button>("ReturnToBodyButton");
                _resurrectInfo = root.Q<Label>("ResurrectInfo");

                if (_resurrectButton != null)
                    _resurrectButton.clicked += OnResurrectClicked;
                if (_releaseSpiritButton != null)
                    _releaseSpiritButton.clicked += OnReleaseSpiritClicked;
                if (_returnToBodyButton != null)
                    _returnToBodyButton.clicked += OnReturnToBodyClicked;

                HideAllScreens();
            }
        }

        void OnDisable()
        {
            if (_resurrectButton != null)
                _resurrectButton.clicked -= OnResurrectClicked;
            if (_releaseSpiritButton != null)
                _releaseSpiritButton.clicked -= OnReleaseSpiritClicked;
            if (_returnToBodyButton != null)
                _returnToBodyButton.clicked -= OnReturnToBodyClicked;
        }

        void Update()
        {
            if (_isDead && !_isGhost)
            {
                _deathTimerValue -= Time.deltaTime;
                if (_deathTimerValue <= 0)
                {
                    _deathTimerValue = 0;
                    if (_resurrectButton != null)
                        _resurrectButton.SetEnabled(false);
                }

                if (_deathTimer != null)
                    _deathTimer.text = $"Воскрешение доступно: {Mathf.CeilToInt(_deathTimerValue)} сек";
            }
        }

        public void OnPlayerDeath(Vector3 corpsePosition)
        {
            _isDead = true;
            _isGhost = false;
            _deathTimerValue = resurrectWindowSeconds;
            _corpsePosition = corpsePosition;

            if (_deathScreen != null)
                _deathScreen.style.display = DisplayStyle.Flex;
            if (_ghostScreen != null)
                _ghostScreen.style.display = DisplayStyle.None;

            if (_resurrectButton != null)
                _resurrectButton.SetEnabled(true);

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
                playerPhysics.enabled = false;

            Debug.Log("Player died. Resurrection window: 60s");
        }

        public void OnResurrectByAlly()
        {
            HideAllScreens();
            _isDead = false;
            _isGhost = false;

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
                playerPhysics.enabled = true;

            Debug.Log("Resurrected by ally!");
        }

        public void OnResurrectByItem()
        {
            HideAllScreens();
            _isDead = false;
            _isGhost = false;

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
                playerPhysics.enabled = true;

            Debug.Log("Resurrected by item!");
        }

        void OnResurrectClicked()
        {
            Debug.Log("Waiting for resurrection...");
        }

        void OnReleaseSpiritClicked()
        {
            _isDead = false;
            _isGhost = true;

            if (_deathScreen != null)
                _deathScreen.style.display = DisplayStyle.None;
            if (_ghostScreen != null)
                _ghostScreen.style.display = DisplayStyle.Flex;

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
            {
                playerPhysics.enabled = true;
                playerPhysics.walkSpeed *= 2f;
                playerPhysics.runSpeed *= 2f;
            }

            var renderer = GetComponent<Renderer>();
            if (renderer != null)
            {
                var mat = renderer.material;
                var color = mat.color;
                color.a = 0.5f;
                mat.color = color;
            }

            Debug.Log("Released spirit. Ghost mode enabled.");
        }

        void OnReturnToBodyClicked()
        {
            if (Vector3.Distance(transform.position, _corpsePosition) > 10f)
            {
                Debug.Log("Too far from corpse!");
                return;
            }

            HideAllScreens();
            _isGhost = false;

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
            {
                playerPhysics.enabled = true;
                playerPhysics.walkSpeed /= 2f;
                playerPhysics.runSpeed /= 2f;
            }

            var renderer = GetComponent<Renderer>();
            if (renderer != null)
            {
                var mat = renderer.material;
                var color = mat.color;
                color.a = 1f;
                mat.color = color;
            }

            Debug.Log("Resurrected at corpse. Durability loss: 10%");
        }

        public void OnResurrectAtSpiritHealer(Vector3 healerPosition)
        {
            HideAllScreens();
            _isGhost = false;

            transform.position = healerPosition;

            var playerPhysics = GetComponent<PlayerPhysics>();
            if (playerPhysics != null)
            {
                playerPhysics.enabled = true;
                playerPhysics.walkSpeed /= 2f;
                playerPhysics.runSpeed /= 2f;
            }

            var renderer = GetComponent<Renderer>();
            if (renderer != null)
            {
                var mat = renderer.material;
                var color = mat.color;
                color.a = 1f;
                mat.color = color;
            }

            Debug.Log("Resurrected at spirit healer. Durability loss: 10%, Sickness debuff applied.");
        }

        void HideAllScreens()
        {
            if (_deathScreen != null)
                _deathScreen.style.display = DisplayStyle.None;
            if (_ghostScreen != null)
                _ghostScreen.style.display = DisplayStyle.None;
        }

        void OnDestroy()
        {
            if (_resurrectButton != null)
                _resurrectButton.clicked -= OnResurrectClicked;
            if (_releaseSpiritButton != null)
                _releaseSpiritButton.clicked -= OnReleaseSpiritClicked;
            if (_returnToBodyButton != null)
                _returnToBodyButton.clicked -= OnReturnToBodyClicked;
        }
    }
}