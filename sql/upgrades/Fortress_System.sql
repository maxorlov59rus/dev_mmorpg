-- Fortress Definitions Table
CREATE TABLE IF NOT EXISTS fort_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    fort_type VARCHAR(32) NOT NULL,
    min_level INT UNSIGNED NOT NULL DEFAULT 1,
    max_level INT UNSIGNED NOT NULL DEFAULT 99,
    construction_cost INT UNSIGNED NOT NULL DEFAULT 0,
    maintenance_cost INT UNSIGNED NOT NULL DEFAULT 0,
    capacity INT UNSIGNED NOT NULL DEFAULT 10,
    fort_benefits TEXT NOT NULL,
    is_capital TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Fort Building Definitions Table
CREATE TABLE IF NOT EXISTS fort_building_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    fort_id INT UNSIGNED NOT NULL,
    building_type VARCHAR(32) NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    build_cost INT UNSIGNED NOT NULL DEFAULT 0,
    build_time INT UNSIGNED NOT NULL DEFAULT 0,
    maintenance_cost INT UNSIGNED NOT NULL DEFAULT 0,
    level_requirement INT UNSIGNED NOT NULL DEFAULT 1,
    effects TEXT NOT NULL,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Fort Buildings Table (player-owned)
CREATE TABLE IF NOT EXISTS fort_buildings (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    fort_id INT UNSIGNED NOT NULL,
    building_type VARCHAR(32) NOT NULL,
    level INT UNSIGNED NOT NULL DEFAULT 1,
    build_time_left INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    UNIQUE KEY unique_player_fort_building (player_guid, fort_id, building_type)
) ENGINE=InnoDB;

-- Garrison Table
CREATE TABLE IF NOT EXISTS fort_garrison (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    fort_id INT UNSIGNED NOT NULL,
    npc_id INT UNSIGNED NOT NULL,
    garrison_type VARCHAR(32) NOT NULL,
    level INT UNSIGNED NOT NULL DEFAULT 1,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

-- Captures Table
CREATE TABLE IF NOT EXISTS fort_captures (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    fort_id INT UNSIGNED NOT NULL,
    captured_by_faction INT UNSIGNED NOT NULL,
    captured_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    released_at TIMESTAMP NULL,
    duration_hours INT UNSIGNED NOT NULL DEFAULT 0,
    points_contributed INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Rankings Table
CREATE TABLE IF NOT EXISTS fort_rankings (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    fort_id INT UNSIGNED NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    total_points INT UNSIGNED NOT NULL DEFAULT 0,
    ranking_period_start TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ranking_period_end TIMESTAMP DEFAULT NULL,
    UNIQUE KEY unique_fort_ranking (fort_id, faction_id)
) ENGINE=InnoDB;
