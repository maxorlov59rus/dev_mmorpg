-- ============================================================================
-- Dungeon_Raid_Matchmaking.sql — Система подбора в данжи и рейды
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. DUNGEON_DEF — Определение данжей
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS dungeon_def (
    id SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    max_level TINYINT UNSIGNED NOT NULL DEFAULT 60,
    min_players TINYINT UNSIGNED NOT NULL DEFAULT 1,
    max_players TINYINT UNSIGNED NOT NULL DEFAULT 5,
    is_raid TINYINT UNSIGNED NOT NULL DEFAULT 0,
    reset_hours INT UNSIGNED NOT NULL DEFAULT 24
) ENGINE=InnoDB;

INSERT IGNORE INTO dungeon_def (id, name_ru, name_en, map_id, min_level, max_level, min_players, max_players, is_raid, reset_hours) VALUES
(20, 'Пещеры Испытаний', 'Trial Caves', 20, 10, 15, 1, 5, 0, 24),
(7, 'Храм забвения', 'Temple of Oblivion', 7, 19, 22, 1, 5, 0, 24),
(9, 'Цитадель тьмы', 'Dark Citadel', 9, 23, 26, 1, 5, 0, 24),
(11, 'Разлом Шип', 'Rift of Spire', 11, 27, 31, 1, 5, 0, 24),
(12, 'Цитадель Искажённых', 'Citadel of Distorted', 12, 32, 36, 1, 5, 0, 24),
(13, 'Мёртвые Пределы', 'Dead Limits', 13, 37, 41, 1, 5, 0, 24),
(14, 'Руины Древних', 'Ancient Ruins', 14, 42, 46, 1, 5, 0, 24),
(15, 'Глотка', 'The Maw', 15, 47, 50, 10, 10, 1, 168),
(100, 'Катакомбы Новичков', 'Novice Catacombs', 100, 1, 9, 1, 3, 0, 12),
(101, 'Логово Волков', 'Wolf Den', 101, 5, 10, 1, 3, 0, 12);

-- ---------------------------------------------------------------------------
-- 2. DUNGEON_DIFFICULTY — Уровни сложности
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS dungeon_difficulty (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
-- ---------------------------------------------------------------------------
-- 4. LFG_QUEUE — Очередь поиска группы
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lfg_queue (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid INT UNSIGNED NOT NULL,
    dungeon_id SMALLINT UNSIGNED NOT NULL,
    difficulty_id TINYINT UNSIGNED NOT NULL DEFAULT 0,
    role_mask TINYINT UNSIGNED NOT NULL DEFAULT 0,
    queue_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    FOREIGN KEY (dungeon_id) REFERENCES dungeon_def(id),
    FOREIGN KEY (difficulty_id) REFERENCES dungeon_difficulty(id),
    INDEX idx_dungeon (dungeon_id),
    INDEX idx_active (is_active)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 5. LFG_GROUP — Группы LFG
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lfg_group (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dungeon_id SMALLINT UNSIGNED NOT NULL,
    difficulty_id TINYINT UNSIGNED NOT NULL DEFAULT 0,
    leader_guid INT UNSIGNED NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status TINYINT UNSIGNED NOT NULL DEFAULT 0,
    FOREIGN KEY (dungeon_id) REFERENCES dungeon_def(id),
    FOREIGN KEY (difficulty_id) REFERENCES dungeon_difficulty(id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 6. LFG_GROUP_MEMBER — Участники группы
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lfg_group_member (
    group_id BIGINT UNSIGNED NOT NULL,
    player_guid INT UNSIGNED NOT NULL,
    role TINYINT UNSIGNED NOT NULL DEFAULT 4,
    joined_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (group_id, player_guid),
    FOREIGN KEY (group_id) REFERENCES lfg_group(id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 7. DUNGEON_INSTANCE — Инстансы данжей
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS dungeon_instance (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dungeon_id SMALLINT UNSIGNED NOT NULL,
    difficulty_id TINYINT UNSIGNED NOT NULL DEFAULT 0,
    map_id SMALLINT UNSIGNED NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    reset_time TIMESTAMP NOT NULL,
    status TINYINT UNSIGNED NOT NULL DEFAULT 0,
    FOREIGN KEY (dungeon_id) REFERENCES dungeon_def(id),
    FOREIGN KEY (difficulty_id) REFERENCES dungeon_difficulty(id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 8. DUNGEON_CONFIG — Конфигурация данжей
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS dungeon_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO dungeon_config (config_key, config_value, comment) VALUES
('lfg_enabled', 'true', 'Включить LFG'),
('lfg_queue_timeout_sec', '300', 'Таймаут очереди LFG (сек)'),
('lfg_group_forming_timeout_sec', '60', 'Таймаут формирования группы (сек)'),
('lfg_auto_accept', 'true', 'Автоматическое принятие группы'),
('difficulty_scaling', 'true', 'Масштабирование сложности'),
('raid_lockout_hours', '168', 'Локдаун рейда (часы)'),
('dungeon_lockout_hours', '24', 'Локдаун данжа (часы)'),
('max_groups_per_dungeon', '10', 'Макс групп на данж'),
('min_tank_per_group', '1', 'Мин танков в группе'),
('min_healer_per_group', '1', 'Мин хилов в группе'),
('min_dps_per_group', '3', 'Мин ДПС в группе');
    name_en VARCHAR(32) NOT NULL,
    hp_multiplier FLOAT NOT NULL DEFAULT 1.0,
    dmg_multiplier FLOAT NOT NULL DEFAULT 1.0,
    loot_multiplier FLOAT NOT NULL DEFAULT 1.0,
    xp_multiplier FLOAT NOT NULL DEFAULT 1.0,
    min_item_level TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO dungeon_difficulty (id, name_ru, name_en, hp_multiplier, dmg_multiplier, loot_multiplier, xp_multiplier, min_item_level, is_active) VALUES
(0, 'Обычный', 'Normal', 1.0, 1.0, 1.0, 1.0, 0, 1),
(1, 'Героический', 'Heroic', 2.0, 1.5, 1.5, 1.2, 30, 1),
(2, 'Эпический', 'Epic', 4.0, 2.0, 2.0, 1.5, 50, 1),
(3, 'Легендарный', 'Legendary', 8.0, 3.0, 3.0, 2.0, 70, 1);

-- ---------------------------------------------------------------------------
-- 3. DUNGEON_BOSS — Боссы данжей
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS dungeon_boss (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dungeon_id SMALLINT UNSIGNED NOT NULL,
    boss_entry INT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    order_index TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_final TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

INSERT IGNORE INTO dungeon_boss (dungeon_id, boss_entry, name_ru, name_en, order_index, is_final) VALUES
(20, 50006, 'Ядовитый паук Кадук', 'Venomous Spider Kaduk', 1, 1),
(7, 60003, 'Древний Лич', 'Ancient Lich', 1, 1),
(9, 60002, 'Повелитель Искажённых', 'Lord of the Distorted', 1, 1),
(11, 60001, 'Страж Разлома', 'Rift Guardian', 1, 1),
(12, 60002, 'Повелитель Искажённых', 'Lord of the Distorted', 1, 1),
(13, 60003, 'Древний Лич', 'Ancient Lich', 1, 1),
(14, 60004, 'Повелитель Древних', 'Lord of the Ancients', 1, 1),
(15, 60005, 'Фрагмент Чёрной Материи', 'Fragment of Black Matter', 1, 1);