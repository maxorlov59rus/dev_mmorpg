-- ============================================================================
-- Spirit_Battle_PvP.sql — PvP ивент "Битва Духов"
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. SPIRIT_BATTLE_CONFIG — Конфигурация ивента
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_battle_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_battle_config (config_key, config_value, comment) VALUES
('event_enabled', 'true', 'Включить ивент'),
('min_level', '20', 'Минимальный уровень'),
('min_spirit_level', '1', 'Минимальный уровень духа'),
('max_participants', '100', 'Макс участников'),
('battle_duration_min', '15', 'Длительность битвы (мин)'),
('queue_duration_min', '5', 'Длительность очереди (мин)'),
('reward_spirit_exp', '500', 'Опыт духа за участие'),
('reward_spirit_exp_win', '1500', 'Опыт духа за победу'),
('daily_limit', '3', 'Лимит боёв в день'),
('entry_fee', '0', 'Вступительный взнос (0=бесплатно)'),
('schedule_day', '6', 'День недели (0=Вс, 6=Сб)'),
('schedule_hour', '18', 'Час начала'),
('schedule_minute', '0', 'Минута начала');

-- ---------------------------------------------------------------------------
-- 2. SPIRIT_BATTLE_SCHEDULE — Расписание
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_battle_schedule (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    day_of_week TINYINT UNSIGNED NOT NULL,
    hour TINYINT UNSIGNED NOT NULL,
    minute TINYINT UNSIGNED NOT NULL,
    duration_min INT UNSIGNED NOT NULL DEFAULT 15,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_battle_schedule (id, day_of_week, hour, minute, duration_min, is_active, comment) VALUES
(1, 6, 18, 0, 15, 1, 'Суббота 18:00 — Битва Духов');

-- ---------------------------------------------------------------------------
-- 3. SPIRIT_BATTLE_REWARDS — Награды
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_battle_rewards (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    placement TINYINT UNSIGNED NOT NULL,
    reward_type VARCHAR(32) NOT NULL,
    reward_id INT UNSIGNED NOT NULL,
    reward_count INT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_battle_rewards (id, placement, reward_type, reward_id, reward_count, comment) VALUES
(1, 1, 'spirit_exp', 0, 1500, '1 место — 1500 опыта духа'),
(2, 2, 'spirit_exp', 0, 1000, '2 место — 1000 опыта духа'),
(3, 3, 'spirit_exp', 0, 750, '3 место — 750 опыта духа'),
(4, 0, 'spirit_exp', 0, 500, 'Участие — 500 опыта духа'),
(5, 1, 'title', 9001, 1, 'Титул "Повелитель Духов"'),
(6, 2, 'title', 9002, 1, 'Титул "Хранитель Духов"'),

-- ---------------------------------------------------------------------------
-- 4. SPIRIT_BATTLE_LOG — Лог боёв
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_battle_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    battle_id BIGINT UNSIGNED NOT NULL,
    player_guid BIGINT UNSIGNED NOT NULL,
    spirit_id BIGINT UNSIGNED NOT NULL,
    placement TINYINT UNSIGNED NOT NULL DEFAULT 0,
    damage_dealt BIGINT UNSIGNED NOT NULL DEFAULT 0,
    healing_done BIGINT UNSIGNED NOT NULL DEFAULT 0,
    kills TINYINT UNSIGNED NOT NULL DEFAULT 0,
    deaths TINYINT UNSIGNED NOT NULL DEFAULT 0,
    participated_at INT UNSIGNED NOT NULL,
    INDEX idx_player (player_guid),
    INDEX idx_battle (battle_id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 5. NPC — Организатор битвы духов
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(94001, 'Организатор Битвы Духов', 'Арена Духов', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(94001, 1, 65536*160, 65536*100, 65536*10, 0x5);

-- ---------------------------------------------------------------------------
-- 6. GOSSIP меню для NPC
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO gossip_menu (ID, TextID, ScriptName) VALUES
(94001, 94001, 'gossip_spirit_battle');

-- ---------------------------------------------------------------------------
-- 7. ДИАЛОГИ NPC (RU/EN)
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO npc_text (ID, Text0_0, Text0_1, BroadcastTextID0, lang0, Probability0, Emote0_0, Emote0_1, Emote0_2, EmoteDelay0_0, EmoteDelay0_1, EmoteDelay0_2) VALUES
(94001, 'Добро пожаловать на Арену Духов! Здесь духи сражаются за честь своих хозяев. Готов ли ты выпустить своего духа в бой?', 'Welcome to the Spirit Arena! Here spirits fight for the honor of their masters. Are you ready to unleash your spirit in battle?', 0, 0, 1, 0, 0, 0, 0, 0, 0);
(7, 3, 'title', 9003, 1, 'Титул "Призыватель"');