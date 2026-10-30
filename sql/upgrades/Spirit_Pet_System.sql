-- ============================================================================
-- Spirit_Pet_System.sql — Система духов-питомцев
-- База: dm_world + dm_characters
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. SPIRIT_QUALITY_DEF — Определение 4 уровней качества духов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_quality_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    color_hex VARCHAR(8) NOT NULL,
    stat_multiplier FLOAT NOT NULL DEFAULT 1.0,
    skill_slots TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_quality_def (id, name_ru, name_en, color_hex, stat_multiplier, skill_slots, comment) VALUES
(1, 'Обычный', 'Common', '#FFFFFF', 1.0, 1, 'Базовый дух'),
(2, 'Редкий', 'Rare', '#00FF00', 1.5, 2, 'Усиленный дух'),
(3, 'Эпический', 'Epic', '#9933FF', 2.0, 3, 'Мощный дух'),
(4, 'Легендарный', 'Legendary', '#FF8000', 3.0, 4, 'Легендарный дух');

-- ---------------------------------------------------------------------------
-- 2. SPIRIT_CLASS_DEF — Специализации духов (как классы персонажей)
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_class_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    role VARCHAR(16) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    icon VARCHAR(64) NOT NULL
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_class_def (id, name_ru, name_en, role, description_ru, description_en, icon) VALUES
(1, 'Страж', 'Guardian', 'tank', 'Танк: привлекает внимание, поглощает урон.', 'Tank: taunts, absorbs damage.', 'icon_guardian'),
(2, 'Целитель', 'Healer', 'healer', 'Хилер: восстанавливает HP.', 'Healer: restores HP.', 'icon_healer'),
(3, 'Разрушитель', 'Destroyer', 'dps', 'ДПС: наносит урон.', 'DPS: deals damage.', 'icon_destroyer'),
(4, 'Поддержка', 'Support', 'support', 'Поддержка: баффы и дебаффы.', 'Support: buffs and debuffs.', 'icon_support');

-- ---------------------------------------------------------------------------
-- 3. SPIRIT_SKILL_DEF — Навыки духов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_skill_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    class_id TINYINT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    damage_type VARCHAR(16) NOT NULL DEFAULT 'physical',
    base_damage INT NOT NULL DEFAULT 0,
    cooldown_ms INT NOT NULL DEFAULT 5000,
    mana_cost INT NOT NULL DEFAULT 0,
    FOREIGN KEY (class_id) REFERENCES spirit_class_def(id)

INSERT IGNORE INTO spirit_skill_def (id, class_id, name_ru, name_en, description_ru, description_en, damage_type, base_damage, cooldown_ms, mana_cost) VALUES
(1, 1, 'Удар щитом', 'Shield Bash', 'Наносит урон и оглушает.', 'Deals damage and stuns.', 'physical', 50, 8000, 20),
(2, 1, 'Провокация', 'Taunt', 'Привлекает внимание врагов.', 'Taunts enemies.', 'physical', 0, 10000, 15),
(3, 1, 'Каменная кожа', 'Stone Skin', 'Увеличивает броню.', 'Increases armor.', 'physical', 0, 15000, 30),
(4, 2, 'Исцеление', 'Heal', 'Восстанавливает HP.', 'Restores HP.', 'holy', 100, 6000, 25),
(5, 2, 'Благословение', 'Blessing', 'Увеличивает силу.', 'Increases strength.', 'holy', 0, 12000, 20),
(6, 2, 'Очищение', 'Purify', 'Снимает дебаффы.', 'Removes debuffs.', 'holy', 0, 10000, 15),
(7, 3, 'Огненный шар', 'Fireball', 'Наносит огненный урон.', 'Deals fire damage.', 'fire', 80, 5000, 20),
(8, 3, 'Ледяная стрела', 'Frost Bolt', 'Наносит ледяной урон и замедляет.', 'Deals frost damage and slows.', 'frost', 60, 6000, 15),
(9, 3, 'Цепная молния', 'Chain Lightning', 'Наносит урон нескольким целям.', 'Deals damage to multiple targets.', 'nature', 40, 8000, 30),
(10, 4, 'Боевой клич', 'Battle Cry', 'Увеличивает урон группы.', 'Increases party damage.', 'physical', 0, 15000, 25),
(11, 4, 'Замедление', 'Slow', 'Замедляет врага.', 'Slows enemy.', 'shadow', 0, 8000, 20),
(12, 4, 'Щит группы', 'Party Shield', 'Защищает группу.', 'Protects party.', 'holy', 0, 20000, 40);

-- ---------------------------------------------------------------------------
-- 4. SPIRIT_CONFIG — Конфигурация
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_config (config_key, config_value, comment) VALUES
('daily_rolls', '10', 'Круток в день'),
('higher_quality_chance', '0.10', 'Шанс выпада высшего уровня'),
('max_spirits_per_player', '5', 'Макс духов на игрока'),
('spirit_sellable', 'false', 'Духов нельзя продавать'),
('quality_1_chance', '0.60', 'Шанс обычного'),
('quality_2_chance', '0.25', 'Шанс редкого'),
('quality_3_chance', '0.12', 'Шанс эпического'),
('quality_4_chance', '0.03', 'Шанс легендарного');

-- ---------------------------------------------------------------------------
-- 5. NPC Хранитель Духов
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(93001, 'Хранитель Духов', 'Мастер Духов', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(93001, 1, 65536*180, 65536*90, 65536*10, 0x5);

-- ---------------------------------------------------------------------------
-- 6. PLAYER_SPIRITS — Духи игроков (в dm_characters)
-- ---------------------------------------------------------------------------
USE dm_characters;

CREATE TABLE IF NOT EXISTS player_spirits (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    quality TINYINT UNSIGNED NOT NULL DEFAULT 1,
    class_id TINYINT UNSIGNED NOT NULL DEFAULT 1,
    level INT UNSIGNED NOT NULL DEFAULT 1,
    experience BIGINT UNSIGNED NOT NULL DEFAULT 0,
    skill_1 INT UNSIGNED DEFAULT NULL,
    skill_2 INT UNSIGNED DEFAULT NULL,
    skill_3 INT UNSIGNED DEFAULT NULL,
    skill_4 INT UNSIGNED DEFAULT NULL,
    rolls_used_today INT UNSIGNED NOT NULL DEFAULT 0,
    last_roll_date INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 0,
    created_at INT UNSIGNED NOT NULL,
    INDEX idx_player (player_guid)
) ENGINE=InnoDB;

-- Духов нельзя продавать — помечаем в item_template
-- (духи не являются предметами, они привязаны к игроку)
) ENGINE=InnoDB;
