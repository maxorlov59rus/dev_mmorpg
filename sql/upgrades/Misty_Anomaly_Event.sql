-- ============================================================================
-- Misty_Anomaly_Event.sql — Еженедельный PvE/PvP эвент "Туманная Аномалия"
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. MISTY_ANOMALY_DEF — Определение эвента
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_def (
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
    max_players TINYINT UNSIGNED NOT NULL DEFAULT 50,
    duration_min INT UNSIGNED NOT NULL DEFAULT 60,
    is_weekly TINYINT UNSIGNED NOT NULL DEFAULT 1,
    day_of_week TINYINT UNSIGNED NOT NULL,
    start_hour TINYINT UNSIGNED NOT NULL DEFAULT 20,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_def (id, name_ru, name_en, map_id, zone_id, pos_x, pos_y, pos_z, min_level, max_level, max_players, duration_min, is_weekly, day_of_week, start_hour, is_active, comment) VALUES
(1, 'Туманная Аномалия', 'Misty Anomaly', 300, 40, 500.0, 250.0, 10.0, 10, 60, 50, 60, 1, 0, 20, 1, 'Еженедельный эвент: Воскресенье 20:00');

-- ---------------------------------------------------------------------------
-- 4. MISTY_ANOMALY_LOOT — Лут мобов аномалии
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_loot (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    loot_table_id INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    chance FLOAT NOT NULL DEFAULT 100.0,
    count INT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_loot (loot_table_id, item_id, chance, count, comment) VALUES
(70001, 15001, 60.0, 1, 'Туманный осколок (ресурс)'),
(70001, 10851, 15.0, 1, 'Зелье исцеления'),
(70002, 15002, 80.0, 1, 'Туманный кристалл (ресурс)'),
(70002, 10852, 10.0, 1, 'Зелье маны'),
(70002, 15003, 5.0, 1, 'Туманный амулет (аксессуар)'),
(70003, 15004, 100.0, 1, 'Туманный артефакт (квестовый)'),
(70003, 15005, 30.0, 1, 'Туманное оружие (Rare)'),
(70003, 15006, 10.0, 1, 'Туманный плащ (Epic)'),
(70004, 15007, 100.0, 1, 'Легендарный туманный артефакт'),
(70004, 15008, 50.0, 1, 'Легендарное туманное оружие'),
(70005, 15009, 70.0, 1, 'Туманная эссенция (ресурс)'),
(70005, 10853, 12.0, 1, 'Зелье силы');

-- ---------------------------------------------------------------------------
-- 5. MISTY_ANOMALY_REWARDS — Награды за участие
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_reward (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    placement TINYINT UNSIGNED NOT NULL,
    currency_type TINYINT UNSIGNED NOT NULL,
    amount INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL DEFAULT 0,
    item_count INT UNSIGNED NOT NULL DEFAULT 0,
    title_id INT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_reward (placement, currency_type, amount, item_id, item_count, title_id, comment) VALUES
(1, 1, 1000, 15007, 1, 2001, '1st place: 1000 gold + legendary artifact + title'),
(2, 1, 500, 15005, 1, 0, '2nd place: 500 gold + rare weapon'),
(3, 1, 250, 15005, 1, 0, '3rd place: 250 gold + rare weapon'),
(0, 1, 50, 15001, 3, 0, 'Participation: 50 gold + 3 misty shards');

-- ---------------------------------------------------------------------------
-- 6. MISTY_ANOMALY_CONFIG — Конфигурация эвента
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_config (config_key, config_value, comment) VALUES
('event_enabled', 'true', 'Включить эвент'),
('min_player_level', '10', 'Минимальный уровень игрока'),
('max_players', '50', 'Макс игроков в зоне'),
('duration_min', '60', 'Длительность эвента (мин)'),
('fog_density', '0.8', 'Плотность тумана (0.0-1.0)'),
('visibility_range', '30', 'Дальность видимости (м)'),
('pvp_enabled', 'true', 'Включить PvP в зоне'),
('mob_spawn_interval_sec', '30', 'Интервал спавна мобов (сек)'),
('boss_spawn_interval_sec', '300', 'Интервал спавна босса (сек)'),
('anomaly_center_x', '500', 'Центр аномалии X'),
('anomaly_center_y', '250', 'Центр аномалии Y'),
('anomaly_center_z', '10', 'Центр аномалии Z'),
('anomaly_radius', '200', 'Радиус аномалии (м)'),
('reward_currency_type', '1', 'Тип валюты награды: 1=gold'),
('participation_reward', '50', 'Награда за участие (золото)'),
('kill_score', '10', 'Очки за убийство моба'),
('pvp_kill_score', '25', 'Очки за убийство игрока'),
('boss_kill_score', '100', 'Очки за убийство босса');
-- ---------------------------------------------------------------------------
-- 2. MISTY_ANOMALY_SCHEDULE — Расписание
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_schedule (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    day_of_week TINYINT UNSIGNED NOT NULL,
    start_hour TINYINT UNSIGNED NOT NULL,
    start_minute TINYINT UNSIGNED NOT NULL DEFAULT 0,
    duration_min INT UNSIGNED NOT NULL,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_schedule (day_of_week, start_hour, start_minute, duration_min, is_active) VALUES
(0, 20, 0, 60, 1);

-- ---------------------------------------------------------------------------
-- 3. MISTY_ANOMALY_MOBS — Мобы аномалии
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS misty_anomaly_mob (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    entry INT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    level TINYINT UNSIGNED NOT NULL,
    type TINYINT UNSIGNED NOT NULL,
    hp_modifier FLOAT NOT NULL DEFAULT 1.0,
    dmg_modifier FLOAT NOT NULL DEFAULT 1.0,
    spawn_count INT UNSIGNED NOT NULL DEFAULT 1,
    spawn_interval_sec INT UNSIGNED NOT NULL DEFAULT 60,
    is_boss TINYINT UNSIGNED NOT NULL DEFAULT 0,
    loot_table_id INT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO misty_anomaly_mob (id, entry, name_ru, name_en, level, type, hp_modifier, dmg_modifier, spawn_count, spawn_interval_sec, is_boss, loot_table_id, comment) VALUES
(1, 70001, 'Туманный призрак', 'Misty Ghost', 15, 4, 1.0, 1.0, 10, 30, 0, 70001, 'Обычный моб'),
(2, 70002, 'Туманный страж', 'Misty Guardian', 20, 6, 2.0, 1.5, 5, 60, 0, 70002, 'Элитный моб'),
(3, 70003, 'Туманный босс', 'Misty Boss', 25, 6, 5.0, 2.0, 1, 300, 1, 70003, 'Босс аномалии'),
(4, 70004, 'Туманный кошмар', 'Misty Nightmare', 30, 8, 8.0, 3.0, 1, 0, 1, 70004, 'Рейд-босс'),
(5, 70005, 'Туманный элементаль', 'Misty Elemental', 18, 4, 1.2, 1.1, 8, 45, 0, 70005, 'Элементальный моб');