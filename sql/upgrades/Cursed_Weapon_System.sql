-- Cursed Weapon System SQL
-- Generated for MMORPG Black Materia

-- 1. Cursed Weapon Definitions Table
CREATE TABLE IF NOT EXISTS cursed_weapon_definitions (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    weapon_type VARCHAR(32) NOT NULL,
    rarity VARCHAR(16) NOT NULL,
    level_requirement INT UNSIGNED NOT NULL DEFAULT 1,
    slot VARCHAR(16) NOT NULL,
    bonus_damage INT UNSIGNED NOT NULL DEFAULT 0,
    bonus_crit INT UNSIGNED NOT NULL DEFAULT 0,
    bonus_accuracy INT UNSIGNED NOT NULL DEFAULT 0,
    bonus_soul_drain INT UNSIGNED NOT NULL DEFAULT 0,
    curse_type VARCHAR(32),
    curse_value INT UNSIGNED NOT NULL DEFAULT 0,
    curse_duration INT UNSIGNED NOT NULL DEFAULT 0,
    curse_frequency INT UNSIGNED NOT NULL DEFAULT 0,
    chance_to_curse INT UNSIGNED NOT NULL DEFAULT 0,
    curse_duration_ticks INT UNSIGNED NOT NULL DEFAULT 0,
    curse_removable TINYINT UNSIGNED NOT NULL DEFAULT 1,
    allowed_factions TEXT,
    restricted_classes TEXT,
    soul_bound TINYINT UNSIGNED NOT NULL DEFAULT 0,
    bind_on_pickup TINYINT UNSIGNED NOT NULL DEFAULT 0,
    visual_effect VARCHAR(64),
    sound_equip VARCHAR(64),
    sound_unequip VARCHAR(64)
) ENGINE=InnoDB;

-- Insert cursed weapon definitions
INSERT IGNORE INTO cursed_weapon_definitions (name_ru, name_en, description_ru, description_en, weapon_type, rarity, level_requirement, slot, 
    bonus_damage, bonus_crit, bonus_accuracy, bonus_soul_drain, curse_type, curse_value, curse_duration, curse_frequency, 
    chance_to_curse, curse_duration_ticks, curse_removable, allowed_factions, restricted_classes, soul_bound, bind_on_pickup, visual_effect, sound_equip, sound_unequip) VALUES
-- Common cursed weapons
('Проклятый клинок', 'Cursed Blade', 'Оружие, порождённое душами нечестивых воинов', 'Cursed blade for wicked warriors', 'sword', 'unusual', 10, 'weapon',
    10, 5, 3, 0, 'speed_reduction', 10, 600, 10, 30, 3, 1, '{"1,2"}', '{}', 0, 0, 'glow_red', 'curse_equip', 'curse_unequip'),
('Тусклый кинжал', 'Dull Dagger', 'Кинжал, передающий часть здоровья врагу', 'Dagger that drains health on hit', 'dagger', 'unusual', 15, 'weapon',
    5, 2, 1, 5, 'health_drain', 5, 120, 15, 20, 3, 1, '{"1,3"}', '{}', 0, 0, 'glow_green', 'curse_equip', 'curse_unequip'),
('Клык Назила', 'Nasira''s Fang', 'Оружие Тёмного Ордена, требующее жертв', 'Fang of Nasira, Dark Order weapon requiring sacrifices', 'sword', 'rare', 40, 'weapon',
    25, 15, 10, 15, 'mana_drain', 15, 300, 20, 40, 5, 1, '{"2"}', '{}', 1, 0, 'glow_purple', 'curse_equip', 'curse_unequip'),
('Шип Теней', 'Serpent of Shadows', 'Оружие, питающееся болью врагов', 'Shadow-serpent weapon that feeds on enemy pain', 'dagger', 'epic', 60, 'weapon',
    35, 20, 15, 20, 'health_drain', 20, 500, 25, 50, 5, 1, '{"1,2,3"}', '{}', 1, 0, 'glow_black', 'curse_equip', 'curse_unequip'),
('Проклятый Колун', 'Cursed Cleaver', 'Оружие, несущее вечное несчастье владельцу', 'Cleaver that brings eternal misfortune to its wielder', 'axe', 'legendary', 80, 'two_hand',
    50, 30, 20, 25, 'speed_reduction', 25, 0, 30, 50, 0, 0, '{"1,2,3"}', '{}', 1, 1, 'glow_white', 'curse_equip', 'curse_unequip');
CREATE TABLE IF NOT EXISTS cursed_weapons_players (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    weapon_entry INT UNSIGNED NOT NULL,
    cursed_def_id INT UNSIGNED NOT NULL,
    level INT UNSIGNED NOT NULL DEFAULT 1,
    curse_power INT UNSIGNED NOT NULL DEFAULT 0,
    curse_remaining_ticks INT UNSIGNED NOT NULL DEFAULT 0,
    is_cursed TINYINT UNSIGNED NOT NULL DEFAULT 1,
    curse_applied_at INT UNSIGNED NOT NULL DEFAULT 0,
    times_cursed INT UNSIGNED NOT NULL DEFAULT 0,
    times_broken INT UNSIGNED NOT NULL DEFAULT 0,
    repair_count INT UNSIGNED NOT NULL DEFAULT 0,
    is_identified TINYINT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_player_weapon (player_guid, weapon_entry)
) ENGINE=InnoDB;

-- Curse Removal Table
CREATE TABLE IF NOT EXISTS curse_removal (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    weapon_entry INT UNSIGNED NOT NULL,
    cure_item_entry INT UNSIGNED NOT NULL,
    success_chance INT UNSIGNED NOT NULL DEFAULT 50,
    uses_remaining INT UNSIGNED NOT NULL DEFAULT 1,
    last_used INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_curse_removal (player_guid, weapon_entry)
) ENGINE=InnoDB;

-- Curse Log Table
CREATE TABLE IF NOT EXISTS curse_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    weapon_entry INT UNSIGNED NOT NULL,
    event_type VARCHAR(32) NOT NULL,
    damage_or_value INT UNSIGNED NOT NULL DEFAULT 0,
    timestamp INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_curse_log_id (id)
) ENGINE=InnoDB;

-- Curse Progression Table
CREATE TABLE IF NOT EXISTS curse_progression (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    cursed_def_id INT UNSIGNED NOT NULL,
    level_range_start INT UNSIGNED NOT NULL DEFAULT 1,
    level_range_end INT UNSIGNED NOT NULL DEFAULT 99,
    damage_penalty INT UNSIGNED NOT NULL DEFAULT 0,
    hp_drain_per_tick INT UNSIGNED NOT NULL DEFAULT 0,
    mana_drain_per_tick INT UNSIGNED NOT NULL DEFAULT 0,
    speed_penalty INT UNSIGNED NOT NULL DEFAULT 0,
    crit_penalty INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_curse_progression (cursed_def_id, level_range_start)
) ENGINE=InnoDB;
