-- ============================================================================
-- Auto_PvP_Ghost_Arena.sql — Ghost Arena: уровни сложности, история матчей,
-- магазин веры и клоны игроков (dm_world)
-- ============================================================================

CREATE TABLE IF NOT EXISTS ghost_arena (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    difficulty TINYINT UNSIGNED NOT NULL DEFAULT 1,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    reward_gold INT UNSIGNED NOT NULL DEFAULT 0,
    reward_faith INT UNSIGNED NOT NULL DEFAULT 0,
    enabled TINYINT UNSIGNED NOT NULL DEFAULT 1,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS ghost_arena_difficulty (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    arena_id INT UNSIGNED NOT NULL,
    level TINYINT UNSIGNED NOT NULL,
    clone_count TINYINT UNSIGNED NOT NULL DEFAULT 1,
    hp_multiplier_q16 INT UNSIGNED NOT NULL DEFAULT 65536,
    damage_multiplier_q16 INT UNSIGNED NOT NULL DEFAULT 65536,
    reward_multiplier_q16 INT UNSIGNED NOT NULL DEFAULT 65536
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS ghost_arena_match (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    arena_id INT UNSIGNED NOT NULL,
    difficulty TINYINT UNSIGNED NOT NULL,
    player_guid INT UNSIGNED NOT NULL,
    won TINYINT UNSIGNED NOT NULL DEFAULT 0,
    duration_sec INT UNSIGNED NOT NULL DEFAULT 0,
    started_at DATETIME NOT NULL,
    finished_at DATETIME NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS ghost_arena_shop (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    item_id INT UNSIGNED NOT NULL,
    price_faith INT UNSIGNED NOT NULL DEFAULT 0,
    daily_limit INT UNSIGNED NOT NULL DEFAULT 0,
    enabled TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS ghost_arena_clone (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    owner_guid INT UNSIGNED NOT NULL,
    source_guid INT UNSIGNED NOT NULL,
    class_id INT UNSIGNED NOT NULL DEFAULT 0,
    level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    snapshot_json TEXT,
    created_at DATETIME NOT NULL,
    expires_at DATETIME NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
