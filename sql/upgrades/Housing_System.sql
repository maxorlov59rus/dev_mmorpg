-- Player Farms table
CREATE TABLE IF NOT EXISTS player_farms (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    farm_id INT UNSIGNED NOT NULL,
    farm_type VARCHAR(32) NOT NULL DEFAULT 'vegetable_garden',
    position_x FLOAT NOT NULL DEFAULT 0,
    position_y FLOAT NOT NULL DEFAULT 0,
    position_z FLOAT NOT NULL DEFAULT 0,
    growth_stage TINYINT UNSIGNED NOT NULL DEFAULT 0,
    harvest_count INT UNSIGNED NOT NULL DEFAULT 0,
    last_harvest INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    INDEX idx_player (player_guid),
    INDEX idx_farm_type (farm_type)
) ENGINE=InnoDB;