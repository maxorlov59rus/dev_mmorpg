-- World_Lore_System.sql — Система мира и лора
-- База: dm_world
USE dm_world;

-- --- Cities ---
CREATE TABLE IF NOT EXISTS city_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    level_range_start TINYINT UNSIGNED NOT NULL,
    level_range_end TINYINT UNSIGNED NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    is_capital TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_unique_city TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- --- NPC Definitions ---
CREATE TABLE IF NOT EXISTS npc_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    title_ru VARCHAR(64) NOT NULL,
    title_en VARCHAR(64) NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    city_id INT UNSIGNED NOT NULL,
    npc_type VARCHAR(32) NOT NULL,
    rank VARCHAR(32) NOT NULL DEFAULT 'citizen',
    gender TINYINT UNSIGNED NOT NULL DEFAULT 0,
    race VARCHAR(32) NOT NULL DEFAULT 'human',
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    is_quest_giver TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_trainer TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_special_interact TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- --- Dialogue Definitions ---
CREATE TABLE IF NOT EXISTS dialogue_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    npc_id INT UNSIGNED NOT NULL,
    dialogue_type VARCHAR(32) NOT NULL,
    text_ru TEXT NOT NULL,
    text_en TEXT NOT NULL,
    emotion VARCHAR(32) NOT NULL DEFAULT 'neutral',
    voice_id INT UNSIGNED,
    priority INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- --- Quest Definitions ---
CREATE TABLE IF NOT EXISTS quest_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    npc_id INT UNSIGNED NOT NULL,
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
) ENGINE=InnoDB;