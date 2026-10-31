-- Weather System SQL Tables
-- Created for MMORPG Black Materia

-- 1. Weather Configuration Table
CREATE TABLE IF NOT EXISTS weather_config (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    weather_type VARCHAR(32) NOT NULL DEFAULT 'clear',
    chance INT UNSIGNED NOT NULL DEFAULT 50,
    duration_min INT UNSIGNED NOT NULL DEFAULT 360,
    duration_max INT UNSIGNED NOT NULL DEFAULT 720,
    transition_min INT UNSIGNED NOT NULL DEFAULT 30,
    is_cycle_enabled TINYINT UNSIGNED NOT NULL DEFAULT 1,
    cycle_interval_min INT UNSIGNED NOT NULL DEFAULT 360,
    sunrise_hour INT UNSIGNED NOT NULL DEFAULT 6,
    sunset_hour INT UNSIGNED NOT NULL DEFAULT 18,
    morning_pressure TINYINT UNSIGNED NOT NULL DEFAULT 70,
    evening_pressure TINYINT UNSIGNED NOT NULL DEFAULT 70,
    wind_speed_base INT UNSIGNED NOT NULL DEFAULT 5,
    wind_gust_chance INT UNSIGNED NOT NULL DEFAULT 20
) ENGINE=InnoDB;

-- Insert default weather configs
INSERT IGNORE INTO weather_config (weather_type, chance, duration_min, duration_max, transition_min, is_cycle_enabled, cycle_interval_min, sunrise_hour, sunset_hour, morning_pressure, evening_pressure, wind_speed_base, wind_gust_chance) VALUES
('clear', 50, 360, 720, 30, 1, 360, 6, 18, 70, 70, 5, 20),
('rain', 30, 240, 480, 30, 1, 360, 6, 18, 75, 75, 8, 35),
('snow', 15, 240, 480, 30, 1, 360, 6, 18, 70, 70, 6, 30),
('thunderstorm', 10, 180, 360, 30, 1, 360, 6, 18, 70, 70, 10, 50),
('fog', 25, 300, 600, 30, 1, 360, 6, 18, 65, 65, 4, 25);

-- 2. World Weather Table
CREATE TABLE IF NOT EXISTS world_weather (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    weather_type VARCHAR(32) NOT NULL DEFAULT 'clear',
    intensity TINYINT UNSIGNED NOT NULL DEFAULT 0,
    area_id INT UNSIGNED NOT NULL,
    start_time INT UNSIGNED NOT NULL,
    end_time INT UNSIGNED NOT NULL DEFAULT 0,
    changed_at INT UNSIGNED NOT NULL DEFAULT 0,
    FOREIGN KEY (area_id) REFERENCES area_def(id)
) ENGINE=InnoDB;

-- 3. Area Weather Table
CREATE TABLE IF NOT EXISTS area_weather (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    area_id INT UNSIGNED NOT NULL,
    weather_type VARCHAR(32) NOT NULL DEFAULT 'clear',
    intensity TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    UNIQUE KEY unique_area_weather (area_id)
) ENGINE=InnoDB;

-- 4. Player Weather Effects Table
CREATE TABLE IF NOT EXISTS player_weather_effects (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    weather_type VARCHAR(32) NOT NULL DEFAULT 'clear',
    intensity TINYINT UNSIGNED NOT NULL DEFAULT 0,
    start_time INT UNSIGNED NOT NULL,
    end_time INT UNSIGNED NOT NULL,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    UNIQUE KEY unique_player_weather (player_guid, weather_type)
) ENGINE=InnoDB;

-- 5. World Events Table
CREATE TABLE IF NOT EXISTS world_events (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    event_type VARCHAR(64) NOT NULL,
    area_id INT UNSIGNED,
    message_ru TEXT NOT NULL,
    message_en TEXT NOT NULL,
    priority INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    expires_at INT UNSIGNED NOT NULL
) ENGINE=InnoDB;