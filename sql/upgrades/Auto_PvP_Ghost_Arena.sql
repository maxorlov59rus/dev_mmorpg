-- ============================================================================
-- Auto_PvP_Ghost_Arena.sql — Авто-PvP с ботами-клонами оффлайн игроков
-- База: dm_world + dm_characters
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. GHOST_ARENA — Специальная зона "Арена Призраков"
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS ghost_arena (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    zone_id TINYINT UNSIGNED NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    max_level TINYINT UNSIGNED NOT NULL DEFAULT 60,
    max_players TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO ghost_arena (id, name_ru, name_en, map_id, zone_id, pos_x, pos_y, pos_z, min_level, max_level, max_players, is_active, comment) VALUES
(1, 'Арена Призраков', 'Ghost Arena', 200, 30, 500.0, 250.0, 10.0, 10, 60, 1, 1, 'Специальная зона для авто-PvP с клонами');

-- ---------------------------------------------------------------------------
-- 2. AUTO_PVP_DIFFICULTY — 3 уровня сложности
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS auto_pvp_difficulty (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    clone_stat_multiplier FLOAT NOT NULL DEFAULT 1.0,
    clone_ai_level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    faith_points_reward INT UNSIGNED NOT NULL DEFAULT 10,
    bonus_faith_points INT UNSIGNED NOT NULL DEFAULT 5,
    min_player_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    entry_fee_gold INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;
-- ---------------------------------------------------------------------------
-- 5. AUTO_PVP_MATCH — История матчей авто-PvP
-- ---------------------------------------------------------------------------
USE dm_world;

CREATE TABLE IF NOT EXISTS auto_pvp_match (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid INT UNSIGNED NOT NULL,
    clone_guid INT UNSIGNED NOT NULL,
    clone_name VARCHAR(24) NOT NULL,
    difficulty_id TINYINT UNSIGNED NOT NULL,
    result TINYINT UNSIGNED NOT NULL,
    faith_points_earned INT UNSIGNED NOT NULL DEFAULT 0,
    match_duration_sec INT UNSIGNED NOT NULL DEFAULT 0,
    match_timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_player (player_guid),
    INDEX idx_timestamp (match_timestamp),
    FOREIGN KEY (difficulty_id) REFERENCES auto_pvp_difficulty(id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 6. FAITH_SHOP — Обменник оружия за очки веры
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS faith_shop_item (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    item_entry INT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT,
    description_en TEXT,
    faith_cost INT UNSIGNED NOT NULL,
    required_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    required_difficulty TINYINT UNSIGNED NOT NULL DEFAULT 1,
    item_level INT UNSIGNED NOT NULL,
    unique_stats_json TEXT,
    is_available TINYINT UNSIGNED NOT NULL DEFAULT 1,
    stock INT UNSIGNED NOT NULL DEFAULT -1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO faith_shop_item (id, item_entry, name_ru, name_en, description_ru, description_en, faith_cost, required_level, required_difficulty, item_level, unique_stats_json, is_available, stock, comment) VALUES
(1, 14001, 'Клинок Призрака', 'Ghost Blade', 'Оружие из Арены Призраков. +10% к урону по клонам.', 'Weapon from Ghost Arena. +10% damage to clones.', 100, 10, 1, 15, '{"damage_vs_clones":0.10}', 1, -1, 'Базовое оружие'),
(2, 14002, 'Посох Призрака', 'Ghost Staff', 'Посох из Арены Призраков. +15% к магическому урону.', 'Staff from Ghost Arena. +15% magical damage.', 150, 15, 1, 20, '{"spell_power_bonus":0.15}', 1, -1, 'Магическое оружие'),
(3, 14003, 'Лук Призрака', 'Ghost Bow', 'Лук из Арены Призраков. +12% к скорости атаки.', 'Bow from Ghost Arena. +12% attack speed.', 120, 12, 1, 18, '{"attack_speed_bonus":0.12}', 1, -1, 'Дальнобойное оружие'),
(4, 14004, 'Кинжал Призрака', 'Ghost Dagger', 'Кинжал из Арены Призраков. +20% к критическому урону.', 'Dagger from Ghost Arena. +20% critical damage.', 200, 20, 2, 25, '{"crit_damage_bonus":0.20}', 1, -1, 'Оружие для разбойников'),
(5, 14005, 'Молот Призрака', 'Ghost Hammer', 'Молот из Арены Призраков. +25% к урону по элитным клонам.', 'Hammer from Ghost Arena. +25% damage to elite clones.', 300, 25, 2, 30, '{"damage_vs_elite":0.25}', 1, -1, 'Оружие для воинов'),
(6, 14006, 'Скипетр Призрака', 'Ghost Scepter', 'Скипетр из Арены Призраков. +30% к исцелению.', 'Scepter from Ghost Arena. +30% healing.', 250, 20, 2, 28, '{"healing_bonus":0.30}', 1, -1, 'Оружие для жрецов'),
(7, 14007, 'Коса Призрака', 'Ghost Scythe', 'Коса из Арены Призраков. +15% к урону и +10% к вампиризму.', 'Scythe from Ghost Arena. +15% damage and +10% lifesteal.', 400, 30, 3, 35, '{"damage_bonus":0.15,"lifesteal":0.10}', 1, -1, 'Оружие для сложного режима'),
(8, 14008, 'Клинок Пустоты', 'Void Blade', 'Легендарный клинок из Арены Призраков. +50% к урону, +20% к криту.', 'Legendary blade from Ghost Arena. +50% damage, +20% crit.', 1000, 40, 3, 45, '{"damage_bonus":0.50,"crit_chance":0.20}', 1, 10, 'Легендарное оружие'),
(9, 14009, 'Посох Пустоты', 'Void Staff', 'Легендарный посох из Арены Призраков. +60% к магическому урону.', 'Legendary staff from Ghost Arena. +60% magical damage.', 1000, 40, 3, 45, '{"spell_power_bonus":0.60}', 1, 10, 'Легендарное оружие'),
(10, 14010, 'Лук Пустоты', 'Void Bow', 'Легендарный лук из Арены Призраков. +40% к скорости атаки, +25% к криту.', 'Legendary bow from Ghost Arena. +40% attack speed, +25% crit.', 1000, 40, 3, 45, '{"attack_speed_bonus":0.40,"crit_chance":0.25}', 1, 10, 'Легендарное оружие');

-- ---------------------------------------------------------------------------
-- 7. AUTO_PVP_CONFIG — Конфигурация авто-PvP
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS auto_pvp_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO auto_pvp_config (config_key, config_value, comment) VALUES
('arena_enabled', 'true', 'Включить Арену Призраков'),
('min_player_level', '10', 'Минимальный уровень игрока'),
('clone_pool_size', '100', 'Размер пула клонов'),
('clone_refresh_hours', '24', 'Обновление пула клонов (часы)'),
('max_matches_per_day', '10', 'Макс матчей в день'),
('faith_points_daily_cap', '500', 'Дневной лимит очков веры'),
('clone_stat_variance', '0.1', 'Вариация статов клонов (±10%)'),
('ai_think_interval_ms', '500', 'Интервал принятия решений ИИ (мс)'),
('match_duration_sec', '120', 'Длительность матча (сек)'),
('respawn_delay_sec', '5', 'Задержка респавна (сек)'),
('ghost_arena_map_id', '200', 'ID карты Арены Призраков'),
('clone_selection_mode', 'random', 'Режим выбора клона: random/level_match/faction_match');

INSERT IGNORE INTO auto_pvp_difficulty (id, name_ru, name_en, clone_stat_multiplier, clone_ai_level, faith_points_reward, bonus_faith_points, min_player_level, entry_fee_gold, is_active, comment) VALUES
(1, 'Лёгкий', 'Easy', 0.8, 1, 10, 5, 10, 0, 1, 'Клоны с пониженными статами, простой ИИ'),
(2, 'Средний', 'Medium', 1.0, 2, 25, 10, 20, 50, 1, 'Клоны с нормальными статами, средний ИИ'),
(3, 'Сложный', 'Hard', 1.2, 3, 50, 25, 30, 100, 1, 'Клоны с повышенными статами, сложный ИИ');

-- ---------------------------------------------------------------------------
-- 3. PLAYER_CLONE — Данные клонов оффлайн игроков
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS player_clone (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid INT UNSIGNED NOT NULL,
    player_name VARCHAR(24) NOT NULL,
    class_id TINYINT UNSIGNED NOT NULL,
    faction_id TINYINT UNSIGNED NOT NULL,
    level TINYINT UNSIGNED NOT NULL,
    equipment_json TEXT NOT NULL,
    stats_json TEXT NOT NULL,
    ai_profile TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_available TINYINT UNSIGNED NOT NULL DEFAULT 1,
    last_updated TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    times_used INT UNSIGNED NOT NULL DEFAULT 0,
    INDEX idx_player (player_guid),
    INDEX idx_available (is_available),
    INDEX idx_level (level)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 4. FAITH_POINTS — Очки веры (валюта за авто-PvP)
-- ---------------------------------------------------------------------------
USE dm_characters;

CREATE TABLE IF NOT EXISTS faith_points (
    guid INT UNSIGNED NOT NULL PRIMARY KEY,
    amount INT UNSIGNED NOT NULL DEFAULT 0,
    total_earned INT UNSIGNED NOT NULL DEFAULT 0,
    total_spent INT UNSIGNED NOT NULL DEFAULT 0,
    last_match TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_amount (amount)
) ENGINE=InnoDB;