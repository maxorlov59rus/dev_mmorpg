-- ============================================================================
-- PvP_Events_Weekly.sql — 10 PvP режимов: еженедельные события
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. PvP_EVENT_DEF — Определение 10 PvP режимов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS pvp_event_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    mode_type TINYINT UNSIGNED NOT NULL,
    min_players TINYINT UNSIGNED NOT NULL DEFAULT 1,
    max_players TINYINT UNSIGNED NOT NULL DEFAULT 10,
    team_size TINYINT UNSIGNED NOT NULL DEFAULT 1,
    duration_min INT UNSIGNED NOT NULL DEFAULT 15,
    is_weekly TINYINT UNSIGNED NOT NULL DEFAULT 1,
    day_of_week TINYINT UNSIGNED NOT NULL,
    start_hour TINYINT UNSIGNED NOT NULL DEFAULT 18,
    map_id SMALLINT UNSIGNED NOT NULL,
    zone_id TINYINT UNSIGNED NOT NULL,
    entry_fee_gold INT UNSIGNED NOT NULL DEFAULT 0,
    reward_currency_type TINYINT UNSIGNED NOT NULL DEFAULT 1,
    reward_amount INT UNSIGNED NOT NULL DEFAULT 100,
    bonus_reward_amount INT UNSIGNED NOT NULL DEFAULT 500,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    max_level TINYINT UNSIGNED NOT NULL DEFAULT 60,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    config_json TEXT
) ENGINE=InnoDB;

INSERT IGNORE INTO pvp_event_def (id, name_ru, name_en, mode_type, min_players, max_players, team_size, duration_min, is_weekly, day_of_week, start_hour, map_id, zone_id, entry_fee_gold, reward_currency_type, reward_amount, bonus_reward_amount, min_level, max_level, is_active, config_json) VALUES
(1, 'Арена 1v1', 'Arena 1v1', 1, 2, 2, 1, 10, 1, 1, 18, 100, 20, 0, 1, 50, 200, 10, 60, 1, '{"normalized_stats":true}'),
(2, 'Арена 3v3', 'Arena 3v3', 2, 6, 6, 3, 15, 1, 2, 18, 101, 21, 0, 1, 100, 500, 10, 60, 1, '{"normalized_stats":true,"rating":true}'),
(3, 'Поле боя 10v10', 'Battleground 10v10', 2, 20, 20, 10, 20, 1, 3, 19, 102, 22, 0, 1, 75, 300, 10, 60, 1, '{"capture_points":3}'),
(4, 'Осада цитадели', 'Citadel Siege', 3, 40, 40, 20, 30, 1, 4, 20, 103, 23, 50, 2, 200, 1000, 20, 60, 1, '{"siege_weapons":true}'),
(5, 'Фронтальная линия', 'Frontline', 4, 60, 60, 1, 45, 1, 5, 18, 104, 24, 0, 3, 150, 750, 10, 60, 1, '{"capture_points":5}'),
(6, 'Караван Порога', 'Threshold Caravan', 5, 30, 30, 1, 25, 1, 6, 17, 105, 25, 0, 4, 100, 500, 10, 60, 1, '{"caravan_hp":50000}'),
(7, 'Дуэльный турнир', 'Duel Tournament', 1, 16, 16, 1, 30, 1, 0, 16, 106, 26, 10, 1, 300, 1500, 10, 60, 1, '{"bracket_size":16}'),
(8, 'Захват флага', 'Capture the Flag', 2, 10, 10, 5, 15, 1, 3, 19, 107, 27, 0, 1, 80, 400, 10, 60, 1, '{"score_limit":3}'),
(9, 'Королевская битва', 'Battle Royale', 1, 100, 100, 1, 20, 1, 6, 21, 108, 28, 0, 5, 500, 2500, 10, 60, 1, '{"zone_shrink":true}'),
(10, 'Арена 2v2', 'Arena 2v2', 2, 4, 4, 2, 10, 1, 4, 18, 109, 29, 0, 1, 60, 300, 10, 60, 1, '{"fast_matches":true}');

-- ---------------------------------------------------------------------------
-- 2. PvP_EVENT_SCHEDULE — Расписание еженедельных событий
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS pvp_event_schedule (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    event_id TINYINT UNSIGNED NOT NULL,
    day_of_week TINYINT UNSIGNED NOT NULL,
    start_hour TINYINT UNSIGNED NOT NULL,
    start_minute TINYINT UNSIGNED NOT NULL DEFAULT 0,
    duration_min INT UNSIGNED NOT NULL,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    FOREIGN KEY (event_id) REFERENCES pvp_event_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO pvp_event_schedule (event_id, day_of_week, start_hour, start_minute, duration_min, is_active) VALUES
(1, 1, 18, 0, 10, 1),
(2, 2, 18, 0, 15, 1),
(3, 3, 19, 0, 20, 1),
(4, 4, 20, 0, 30, 1),
(5, 5, 18, 0, 45, 1),
(6, 6, 17, 0, 25, 1),
(7, 0, 16, 0, 30, 1),
(8, 3, 20, 0, 15, 1),
(9, 6, 21, 0, 20, 1),
(10, 4, 18, 0, 10, 1);