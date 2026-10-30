// QuestMonster.cs - Monster controller for quest mobs in Unity
using UnityEngine;
using UnityEngine.UIElements;

namespace DM.Client
{
    /// <summary>
    /// Controls quest monster behavior, stats, and combat.
    /// </summary>
    public class QuestMonster : MonoBehaviour
    {
        [Header("Monster Stats")]
        public string monsterName = "Mob";
        public int level = 16;
        public float health = 100f;
        public float maxHealth = 100f;
        public float damage = 10f;
        public float moveSpeed = 3f;
        public float attackRange = 2f;
        public float aggroRange = 10f;

        [Header("Combat")]
        public float attackCooldown = 2f;
        private float _lastAttackTime = 0f;

        [Header("Movement")]
        public float wanderRadius = 5f;
        private Vector3 _spawnPosition;
        private Vector3 _targetPosition;
        private float _wanderTimer = 0f;

        [Header("UI")]
        private Label _healthLabel;
        private VisualElement _healthBar;

        private Transform _playerTransform;
        private bool _isAggro = false;
        private bool _isDead = false;

        void Start()
        {
            _spawnPosition = transform.position;
            _targetPosition = _spawnPosition;

            // Find player
            var player = GameObject.FindWithTag("Player");
            if (player != null)
                _playerTransform = player.transform;

            // Setup health bar from UIDocument if available
            var uiDoc = GetComponent<UIDocument>();
            if (uiDoc != null)
            {
                var root = uiDoc.rootVisualElement;
                _healthLabel = root.Q<Label>("HealthLabel");
                _healthBar = root.Q<VisualElement>("HealthBar");
            }

            UpdateHealthUI();
        }

        void Update()
        {
            if (_isDead) return;

            // Check for player in aggro range
            if (_playerTransform != null)
            {
                float distanceToPlayer = Vector3.Distance(transform.position, _playerTransform.position);
                if (distanceToPlayer <= aggroRange)
                {
                    _isAggro = true;
                }
                else if (distanceToPlayer > aggroRange * 1.5f)
                {
                    _isAggro = false;
                }
            }

            if (_isAggro && _playerTransform != null)
            {
                // Move towards player
                Vector3 direction = (_playerTransform.position - transform.position).normalized;
                direction.y = 0;
                transform.position += direction * moveSpeed * Time.deltaTime;

                // Face player
                if (direction != Vector3.zero)
                    transform.rotation = Quaternion.LookRotation(direction);

                // Attack if in range
                float distanceToPlayer = Vector3.Distance(transform.position, _playerTransform.position);
                if (distanceToPlayer <= attackRange)
                {
                    Attack();
                }
            }
            else
            {
                // Wander
                Wander();
            }
        }

        void Wander()
        {
            _wanderTimer -= Time.deltaTime;
            if (_wanderTimer <= 0f)
            {
                Vector2 randomCircle = Random.insideUnitCircle * wanderRadius;
                _targetPosition = _spawnPosition + new Vector3(randomCircle.x, 0, randomCircle.y);
                _wanderTimer = Random.Range(3f, 6f);
            }

            Vector3 direction = (_targetPosition - transform.position).normalized;
            direction.y = 0;
            if (direction != Vector3.zero && Vector3.Distance(transform.position, _targetPosition) > 0.5f)
            {
                transform.position += direction * (moveSpeed * 0.5f) * Time.deltaTime;
                transform.rotation = Quaternion.LookRotation(direction);
            }
        }

        void Attack()
        {
            if (Time.time - _lastAttackTime < attackCooldown) return;

            _lastAttackTime = Time.time;
            Debug.Log($"{monsterName} attacks for {damage} damage!");

            // TODO: Apply damage to player
        }

        public void TakeDamage(float amount)
        {
            if (_isDead) return;

            health -= amount;
            if (health <= 0)
            {
                health = 0;
                Die();
            }

            UpdateHealthUI();
            Debug.Log($"{monsterName} takes {amount} damage. HP: {health}/{maxHealth}");
        }

        void Die()
        {
            _isDead = true;
            Debug.Log($"{monsterName} died!");

            // TODO: Drop loot
            // TODO: Grant XP to player
            // TODO: Update quest progress

            // Destroy after delay
            Destroy(gameObject, 3f);
        }

        void UpdateHealthUI()
        {
            if (_healthLabel != null)
                _healthLabel.text = $"{health:F0}/{maxHealth:F0}";

            if (_healthBar != null)
            {
                float healthPercent = health / maxHealth;
                _healthBar.style.width = Length.Percent(healthPercent * 100);
            }
        }
    }
}