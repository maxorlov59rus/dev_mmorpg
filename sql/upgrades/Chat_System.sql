-- ============================================================================
-- Chat_System.sql — Система чатов и приватных сообщений
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ----------------------------------------------------------------------------
-- 1. CHAT_DEFINITIONS — Типы чатов и конфигурация
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS chat_definitions (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    chat_type VARCHAR(32) NOT NULL, -- 'global', 'region', 'faction', 'party', 'whisper', 'instance'
    level_requirement INT UNSIGNED NOT NULL DEFAULT 0,
    max_participants INT UNSIGNED NOT NULL DEFAULT 0,
    is_public TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_logging TINYINT UNSIGNED NOT NULL DEFAULT 1,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

INSERT IGNORE INTO chat_definitions (name_ru, name_en, chat_type, level_requirement, max_participants, is_public, is_logging, sort_order) VALUES
('Глобальный', 'Global', 'global', 0, 0, 1, 1, 1),
('Региональный', 'Regional', 'region', 10, 100, 1, 1, 2),
('Фракционный', 'Faction', 'faction', 1, 50, 1, 1, 3),
('Партийный', 'Party', 'party', 1, 5, 1, 1, 4),
('Личный (шепот)', 'Whisper', 'whisper', 0, 1, 0, 1, 5),
('Инстансовый', 'Instance', 'instance', 1, 25, 0, 1, 6),
('Гильдейский', 'Guild', 'guild', 5, 20, 1, 1, 7);

-- ----------------------------------------------------------------------------
-- 2. CHAT_CHANNELS — Активные чат-каналы
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS chat_channels (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    channel_name VARCHAR(64) NOT NULL,
    chat_type VARCHAR(32) NOT NULL,
    creator_guid BIGINT UNSIGNED,
    created_at INT UNSIGNED NOT NULL,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    topic VARCHAR(128) DEFAULT NULL,
    INDEX idx_type (chat_type),
    INDEX idx_active (is_active)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 3. CHAT_MESSAGES — История сообщений
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS chat_messages (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    channel_id BIGINT UNSIGNED NOT NULL,
    player_guid BIGINT UNSIGNED NOT NULL,
    message_text TEXT NOT NULL,
    message_timestamp INT UNSIGNED NOT NULL,
    is_whisper TINYINT UNSIGNED NOT NULL DEFAULT 0,
    target_player_guid BIGINT UNSIGNED DEFAULT NULL,
    is_system TINYINT UNSIGNED NOT NULL DEFAULT 0,
    INDEX idx_channel (channel_id),
    idx_player (player_guid),
    idx_target (target_player_guid),
    idx_timestamp (message_timestamp)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 4. CHAT_SETTINGS — Настройки чата для игроков
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS chat_settings (
    player_guid BIGINT UNSIGNED NOT NULL,
    channel_id BIGINT UNSIGNED NOT NULL,
    is_muted TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_blocked TINYINT UNSIGNED NOT NULL DEFAULT 0,
    mute_until INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_player_channel (player_guid, channel_id)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 5. FRIENDS_LIST — Список друзей для приватных сообщений
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS friends_list (
    player_guid BIGINT UNSIGNED NOT NULL,
    friend_guid BIGINT UNSIGNED NOT NULL,
    added_at INT UNSIGNED NOT NULL,
    UNIQUE KEY unique_friendship (player_guid, friend_guid)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 6. IGNORE_LIST — Список игнорируемых игроков
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS ignore_list (
    player_guid BIGINT UNSIGNED NOT NULL,
    ignored_guid BIGINT UNSIGNED NOT NULL,
    added_at INT UNSIGNED NOT NULL,
    UNIQUE KEY unique_ignore (player_guid, ignored_guid)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 7. CHAT_CONFIG — Глобальная конфигурация чата
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS chat_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO chat_config (config_key, config_value, comment) VALUES
('chat_enabled', 'true', 'Включить систему чата'),
('max_message_length', '255', 'Максимальная длина сообщения'),
('filter_profanity', 'true', 'Фильтровать неценность'),
('whisper_cooldown', '30', 'Кулдаун между шепотами (сек)'),
('global_chat_level', '0', 'Уровень для доступа к глобальному чату (0=все)');