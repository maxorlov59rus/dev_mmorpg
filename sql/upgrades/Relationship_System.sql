-- ============================================================================
-- Relationship_System.sql — Система отношений: друзья, пара, брак
-- База: dm_world + dm_characters
-- ============================================================================

USE dm_characters;

-- ---------------------------------------------------------------------------
-- 1. Добавление поля пола в таблицу персонажей
-- ---------------------------------------------------------------------------
ALTER TABLE characters ADD COLUMN IF NOT EXISTS gender TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=мужской, 1=женский';

-- ---------------------------------------------------------------------------
-- 2. FRIENDSHIP — Список друзей
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS friendship (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    friend_guid BIGINT UNSIGNED NOT NULL,
    status TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=pending, 1=accepted, 2=blocked',
    created_at INT UNSIGNED NOT NULL,
    accepted_at INT UNSIGNED DEFAULT NULL,
    UNIQUE KEY unique_friendship (player_guid, friend_guid),
    INDEX idx_player (player_guid),
    INDEX idx_friend (friend_guid)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 3. RELATIONSHIP — Отношения (пара, брак)
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS relationship (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player1_guid BIGINT UNSIGNED NOT NULL,
    player2_guid BIGINT UNSIGNED NOT NULL,
    type TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=знакомые, 1=пара, 2=помолвка, 3=брак',
    status TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=pending, 1=active, 2=dissolved',
    created_at INT UNSIGNED NOT NULL,
    accepted_at INT UNSIGNED DEFAULT NULL,
    dissolved_at INT UNSIGNED DEFAULT NULL,
    UNIQUE KEY unique_relationship (player1_guid, player2_guid),
    INDEX idx_player1 (player1_guid),
    INDEX idx_player2 (player2_guid)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 4. RELATIONSHIP_CONFIG — Конфигурация
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS relationship_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO relationship_config (config_key, config_value, comment) VALUES
('friendship_enabled', 'true', 'Включить систему друзей'),
('relationship_enabled', 'true', 'Включить систему отношений'),
('max_friends', '50', 'Максимум друзей'),
('max_relationships', '1', 'Максимум активных отношений'),
('couple_level_req', '10', 'Минимальный уровень для пары'),
('marriage_level_req', '20', 'Минимальный уровень для брака'),
('friend_request_cooldown', '60', 'Кулдаун запроса дружбы (сек)'),
('relationship_request_cooldown', '300', 'Кулдаун запроса отношений (сек)');

-- ---------------------------------------------------------------------------
-- 5. GENDER_DEF — Определение полов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS gender_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    icon VARCHAR(64) NOT NULL
) ENGINE=InnoDB;

INSERT IGNORE INTO gender_def (id, name_ru, name_en, icon) VALUES
(0, 'Мужской', 'Male', 'icon_male'),
(1, 'Женский', 'Female', 'icon_female');

USE dm_world;

-- ---------------------------------------------------------------------------
-- 6. RELATIONSHIP_NPC — NPC для отношений (сваха)
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(92001, 'Сваха Мария', 'Хранительница Сердец', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(92001, 1, 65536*150, 65536*120, 65536*10, 0x5);