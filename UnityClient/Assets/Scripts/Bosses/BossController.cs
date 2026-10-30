// BossController.cs — Контроллер боссов в Unity (визуализация, механики, фазы)
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    /// <summary>
    /// Controls boss behavior, phases, abilities, and visual effects.
    /// </summary>
    public class BossController : MonoBehaviour
    {
        [Header("Boss Info")]
        public string bossName = "Boss";
        public int bossEntry = 60001;
        public int level = 30;
        public float maxHealth = 5000f;
        public float currentHealth = 5000f;

        [Header("Phases")]
        public float phase2HpPercent = 0.5f;
        public bool isPhase2 = false;
        public bool isEnraged = false;

        [Header("Combat")]
        public float attackRange = 3f;
        public float moveSpeed = 2f;
        public float abilityCooldown = 5f;
        private float _lastAbilityTime = 0f;

        [Header("Abilities")]
        public BossAbility[] abilities;

        [Header("Visual")]
        public GameObject phase2Visual;
        public GameObject enrageVisual;
        public ParticleSystem deathEffect;

        [Header("UI")]
        private Label _bossNameLabel;
        private Label _bossHealthLabel;
        private VisualElement _bossHealthBar;
        private Label _phaseLabel;
        private UIDocument _uiDocument;

        private Transform _playerTransform;
        private bool _isDead = false;

        [System.Serializable]
        public class BossAbility
        {
            public string name;
            public float cooldown;
            public float damage;
            public float radius;
            public bool isAoe;
            public GameObject vfxPrefab;
        }

        void Start()
        {
            _playerTransform = GameObject.FindWithTag("Player")?.transform;

            _uiDocument = GetComponent<UIDocument>();
            if (_uiDocument != null)
            {
                var root = _uiDocument.rootVisualElement;
                _bossNameLabel = root.Q<Label>("BossName");
                _bossHealthLabel = root.Q<Label>("BossHealth");
                _bossHealthBar = root.Q<VisualElement>("BossHealthBar");
                _phaseLabel = root.Q<Label>("PhaseLabel");

                if (_bossNameLabel != null)
                    _bossNameLabel.text = bossName;
            }

            UpdateHealthUI();
        }

        void Update()
        {
            if (_isDead) return;

            // Check phase transition
            float hpPercent = currentHealth / maxHealth;
            if (!isPhase2 && hpPercent <= phase2HpPercent)
            {
                EnterPhase2();
            }

            // Use abilities
            if (Time.time - _lastAbilityTime >= abilityCooldown)
            {
                UseRandomAbility();
                _lastAbilityTime = Time.time;
            }

            // Move towards player if aggro
            if (_playerTransform != null)
            {
                float distance = Vector3.Distance(transform.position, _playerTransform.position);
                if (distance > attackRange)
                {
                    Vector3 direction = (_playerTransform.position - transform.position).normalized;
                    direction.y = 0;
                    transform.position += direction * moveSpeed * Time.deltaTime;
                    if (direction != Vector3.zero)
                        transform.rotation = Quaternion.LookRotation(direction);
                }
            }
        }

        void EnterPhase2()
        {
            isPhase2 = true;
            Debug.Log($"{bossName} enters Phase 2!");

            if (phase2Visual != null)
                phase2Visual.SetActive(true);

            if (_phaseLabel != null)
                _phaseLabel.text = "Phase 2";
        }

        void UseRandomAbility()
        {
            if (abilities == null || abilities.Length == 0) return;

            var ability = abilities[Random.Range(0, abilities.Length)];
            Debug.Log($"{bossName} uses {ability.name}!");

            if (ability.vfxPrefab != null)
            {
                Instantiate(ability.vfxPrefab, transform.position, transform.rotation);
            }

            // Apply damage to player if in range
            if (_playerTransform != null)
            {
                float distance = Vector3.Distance(transform.position, _playerTransform.position);
                if (distance <= ability.radius || !ability.isAoe)
                {
                    // TODO: Apply damage to player
                    Debug.Log($"Player takes {ability.damage} damage from {ability.name}");
                }
            }
        }

        public void TakeDamage(float amount)
        {
            if (_isDead) return;

            currentHealth -= amount;
            if (currentHealth <= 0)
            {
                currentHealth = 0;
                Die();
            }

            UpdateHealthUI();
        }

        void Die()
        {
            _isDead = true;
            Debug.Log($"{bossName} defeated!");

            if (deathEffect != null)
                Instantiate(deathEffect, transform.position, transform.rotation);

            // TODO: Drop loot
            // TODO: Grant XP and reputation
            // TODO: Update quest progress

            Destroy(gameObject, 5f);
        }

        void UpdateHealthUI()
        {
            if (_bossHealthLabel != null)
                _bossHealthLabel.text = $"{currentHealth:F0} / {maxHealth:F0}";

            if (_bossHealthBar != null)
            {
                float healthPercent = currentHealth / maxHealth;
                _bossHealthBar.style.width = Length.Percent(healthPercent * 100);
            }
        }

        void OnDestroy()
        {
            // Cleanup
        }
    }
}