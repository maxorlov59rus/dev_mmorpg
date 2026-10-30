-- ============================================================================
-- Beginner_Buff.sql — Бафф новичка для игроков 1-10 уровня
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. BEGINNER_BUFF_DEF — Определение баффа новичка
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS beginner_buff_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    icon_path VARCHAR(128) NOT NULL,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    max_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO beginner_buff_def (id, name_ru, name_en, description_ru, description_en, icon_path, min_level, max_level, is_active) VALUES
(1, 'Благословение Новичка', 'Beginner Blessing', 'Бафф для новых игроков: +50% к опыту, +25% к дропу, +25% к золоту, +10% к скорости передвижения.', 'Buff for new players: +50% XP, +25% drop, +25% gold, +10% movement speed.', 'Icons/Buffs/beginner_blessing', 1, 10, 1);

-- ---------------------------------------------------------------------------
-- 2. BEGINNER_BUFF_STATS — Статы баффа
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS beginner_buff_stats (
    buff_id INT UNSIGNED NOT NULL,
    stat_name VARCHAR(32) NOT NULL,
    stat_value FLOAT NOT NULL,
    PRIMARY KEY (buff_id, stat_name),
    FOREIGN KEY (buff_id) REFERENCES beginner_buff_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO beginner_buff_stats (buff_id, stat_name, stat_value) VALUES
(1, 'xp_multiplier', 1.5),
(1, 'drop_multiplier', 1.25),
(1, 'gold_multiplier', 1.25),
(1, 'movement_speed', 1.1),
(1, 'damage_reduction', 0.1);

-- ---------------------------------------------------------------------------
-- 3. BEGINNER_BUFF_CONFIG — Конфигурация
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS beginner_buff_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO beginner_buff_config (config_key, config_value, comment) VALUES
('beginner_buff_enabled', 'true', 'Включить бафф новичка'),
('beginner_buff_level_cap', '10', 'Максимальный уровень для баффа'),
('beginner_buff_auto_apply', 'true', 'Автоматически применять при входе'),
('beginner_buff_show_ui', 'true', 'Показывать в UI'),
('beginner_buff_remove_on_level_11', 'true', 'Снимать при достижении 11 уровня');

-- ---------------------------------------------------------------------------
-- 4. BEGINNER_BUFF_LOG — Лог применения/снятия баффа
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS beginner_buff_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    action VARCHAR(16) NOT NULL COMMENT 'applied/removed',
    player_level TINYINT UNSIGNED NOT NULL,
    logged_at INT UNSIGNED NOT NULL,
    INDEX idx_player (player_guid)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 5. NPC — Наставник Новичков
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(97001, 'Наставник Новичков', 'Хранитель Благословения', 1, 7, 1650, 1651, 1, 1, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(97001, 1, 65536*50, 65536*50, 65536*10, 0x5);