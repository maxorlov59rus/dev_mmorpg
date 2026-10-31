-- Contamination System SQL Tables
USE dm_world;

-- Table 1: Contamination Zones
CREATE TABLE IF NOT EXISTS contamination_zones (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    location_x FLOAT NOT NULL DEFAULT 0,
    location_y FLOAT NOT NULL DEFAULT 0,
    location_z FLOAT NOT NULL DEFAULT 0,
    radius FLOAT NOT NULL DEFAULT 50,
    contamination_level FLOAT NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    spawn_chance TINYINT UNSIGNED NOT NULL DEFAULT 10,
    respawn_time INT UNSIGNED NOT NULL DEFAULT 300,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Table 2: Player Infection
CREATE TABLE IF NOT EXISTS player_infection (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    zone_id INT UNSIGNED NOT NULL,
    infection_level FLOAT NOT NULL DEFAULT 0,
    start_time INT UNSIGNED NOT NULL,
    last_update INT UNSIGNED NOT NULL DEFAULT 0,
    is_cured TINYINT UNSIGNED NOT NULL DEFAULT 0,
    cure_time INT UNSIGNED DEFAULT NULL,
    UNIQUE KEY unique_player_infection (player_guid, zone_id),
    INDEX idx_player (player_guid),
    INDEX idx_zone (zone_id)
) ENGINE=InnoDB;

-- Table 3: Infection Debuffs
CREATE TABLE IF NOT EXISTS infection_debuffs (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    infection_level_min INT UNSIGNED NOT NULL,
    infection_level_max INT UNSIGNED,
    debuff_type VARCHAR(32) NOT NULL,
    debuff_value FLOAT NOT NULL,
    duration_minutes INT UNSIGNED NOT NULL,
    is_stackable TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

INSERT IGNORE INTO infection_debuffs (infection_level_min, infection_level_max, debuff_type, debuff_value, duration_minutes, is_stackable, sort_order) VALUES
(0, 20, 'speed', 0.3, 15, 1, 1),
(20, 50, 'strength', -0.2, 20, 0, 2),
(50, 80, 'hp_regen', -0.1, 25, 0, 3),
(80, 100, 'crit_chance', -0.15, 30, 0, 4);