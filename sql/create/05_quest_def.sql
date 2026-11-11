-- ============================================================================
-- 05_quest_def.sql — определения квестов (dm_world)
-- Вынесено из sql/upgrades/World_Lore_System.sql: контент в 50-60_Content.sql
-- вставляет строки в quest_def, поэтому таблица должна существовать раньше.
-- ============================================================================

CREATE TABLE IF NOT EXISTS quest_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    npc_id INT UNSIGNED NOT NULL DEFAULT 0,
    city_id INT UNSIGNED,
    faction_id INT UNSIGNED,
    title_ru VARCHAR(64) NOT NULL,
    title_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    level_min TINYINT UNSIGNED NOT NULL,
    level_max TINYINT UNSIGNED NOT NULL,
    quest_type VARCHAR(32) NOT NULL,
    quest_category VARCHAR(32) NOT NULL,
    objectives TEXT NOT NULL,
    reward_type VARCHAR(32) NOT NULL DEFAULT 'gold',
    reward_value VARCHAR(128) NOT NULL DEFAULT '0',
    reward_amount INT UNSIGNED NOT NULL DEFAULT 0,
    time_limit INT UNSIGNED DEFAULT 0,
    is_repeatable TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
