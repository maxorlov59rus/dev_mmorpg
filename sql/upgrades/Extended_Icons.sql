-- ============================================================================
-- Extended_Icons.sql — Иконки навыков, пассивок, вещей, оружия, брони
-- База: dm_world
-- ============================================================================

USE dm_world;

CREATE TABLE IF NOT EXISTS extended_icon_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    file_path VARCHAR(256) NOT NULL,
    category VARCHAR(32) NOT NULL,
    subcategory VARCHAR(32),
    size_px INT UNSIGNED NOT NULL DEFAULT 64,
    quality TINYINT UNSIGNED DEFAULT 0,
    description_ru VARCHAR(255),
    description_en VARCHAR(255),
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

-- Навыки (активные)
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(1, 'icon_skill_shield_bash', 'Icons/Skills/shield_bash', 'skill', 'physical', 64, 0, 'Удар щитом', 'Shield Bash', 1),
(2, 'icon_skill_taunt', 'Icons/Skills/taunt', 'skill', 'physical', 64, 0, 'Провокация', 'Taunt', 1),
(3, 'icon_skill_stone_skin', 'Icons/Skills/stone_skin', 'skill', 'buff', 64, 0, 'Каменная кожа', 'Stone Skin', 1),
(4, 'icon_skill_heal', 'Icons/Skills/heal', 'skill', 'healing', 64, 0, 'Исцеление', 'Heal', 1),
(5, 'icon_skill_blessing', 'Icons/Skills/blessing', 'skill', 'buff', 64, 0, 'Благословение', 'Blessing', 1),
(6, 'icon_skill_purify', 'Icons/Skills/purify', 'skill', 'healing', 64, 0, 'Очищение', 'Purify', 1),
(7, 'icon_skill_fireball', 'Icons/Skills/fireball', 'skill', 'magical', 64, 0, 'Огненный шар', 'Fireball', 1),
(8, 'icon_skill_frost_bolt', 'Icons/Skills/frost_bolt', 'skill', 'magical', 64, 0, 'Ледяная стрела', 'Frost Bolt', 1),
(9, 'icon_skill_chain_lightning', 'Icons/Skills/chain_lightning', 'skill', 'magical', 64, 0, 'Цепная молния', 'Chain Lightning', 1),
(10, 'icon_skill_battle_cry', 'Icons/Skills/battle_cry', 'skill', 'buff', 64, 0, 'Боевой клич', 'Battle Cry', 1),
(11, 'icon_skill_slow', 'Icons/Skills/slow', 'skill', 'debuff', 64, 0, 'Замедление', 'Slow', 1),
(12, 'icon_skill_party_shield', 'Icons/Skills/party_shield', 'skill', 'buff', 64, 0, 'Щит группы', 'Party Shield', 1),
(13, 'icon_skill_backstab', 'Icons/Skills/backstab', 'skill', 'physical', 64, 0, 'Удар в спину', 'Backstab', 1),
(14, 'icon_skill_shadow_step', 'Icons/Skills/shadow_step', 'skill', 'physical', 64, 0, 'Шаг тени', 'Shadow Step', 1),
(15, 'icon_skill_aimed_shot', 'Icons/Skills/aimed_shot', 'skill', 'physical', 64, 0, 'Прицельный выстрел', 'Aimed Shot', 1),
(16, 'icon_skill_multishot', 'Icons/Skills/multishot', 'skill', 'physical', 64, 0, 'Мульти-выстрел', 'Multishot', 1),
(17, 'icon_skill_shadow_bolt', 'Icons/Skills/shadow_bolt', 'skill', 'magical', 64, 0, 'Стрела тьмы', 'Shadow Bolt', 1),

-- Пассивные навыки
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(19, 'icon_passive_toughness', 'Icons/Passive/toughness', 'passive', 'defense', 64, 0, 'Стойкость', 'Toughness', 1),
(20, 'icon_passive_strength', 'Icons/Passive/strength', 'passive', 'offense', 64, 0, 'Сила', 'Strength', 1),
(21, 'icon_passive_agility', 'Icons/Passive/agility', 'passive', 'offense', 64, 0, 'Ловкость', 'Agility', 1),
(22, 'icon_passive_intellect', 'Icons/Passive/intellect', 'passive', 'magic', 64, 0, 'Интеллект', 'Intellect', 1),
(23, 'icon_passive_vitality', 'Icons/Passive/vitality', 'passive', 'defense', 64, 0, 'Выносливость', 'Vitality', 1),
(24, 'icon_passive_spirit', 'Icons/Passive/spirit', 'passive', 'magic', 64, 0, 'Дух', 'Spirit', 1),
(25, 'icon_passive_critical', 'Icons/Passive/critical', 'passive', 'offense', 64, 0, 'Критический удар', 'Critical Strike', 1),
(26, 'icon_passive_dodge', 'Icons/Passive/dodge', 'passive', 'defense', 64, 0, 'Уклонение', 'Dodge', 1),
(27, 'icon_passive_block', 'Icons/Passive/block', 'passive', 'defense', 64, 0, 'Блок', 'Block', 1),
(28, 'icon_passive_regeneration', 'Icons/Passive/regeneration', 'passive', 'healing', 64, 0, 'Регенерация', 'Regeneration', 1);

-- Оружие
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(29, 'icon_weapon_sword_iron', 'Icons/Weapons/sword_iron', 'weapon', 'sword', 64, 1, 'Железный меч', 'Iron Sword', 1),
(30, 'icon_weapon_sword_steel', 'Icons/Weapons/sword_steel', 'weapon', 'sword', 64, 2, 'Стальной меч', 'Steel Sword', 1),
(31, 'icon_weapon_sword_mithril', 'Icons/Weapons/sword_mithril', 'weapon', 'sword', 64, 3, 'Мифриловый меч', 'Mithril Sword', 1),
(32, 'icon_weapon_sword_titanium', 'Icons/Weapons/sword_titanium', 'weapon', 'sword', 64, 4, 'Титановый меч', 'Titanium Sword', 1),
(33, 'icon_weapon_shield_iron', 'Icons/Weapons/shield_iron', 'weapon', 'shield', 64, 1, 'Железный щит', 'Iron Shield', 1),
(34, 'icon_weapon_shield_steel', 'Icons/Weapons/shield_steel', 'weapon', 'shield', 64, 2, 'Стальной щит', 'Steel Shield', 1),
(35, 'icon_weapon_shield_mithril', 'Icons/Weapons/shield_mithril', 'weapon', 'shield', 64, 3, 'Мифриловый щит', 'Mithril Shield', 1),
(36, 'icon_weapon_shield_titanium', 'Icons/Weapons/shield_titanium', 'weapon', 'shield', 64, 4, 'Титановый щит', 'Titanium Shield', 1),
(37, 'icon_weapon_dagger', 'Icons/Weapons/dagger', 'weapon', 'dagger', 64, 2, 'Кинжал', 'Dagger', 1),
(38, 'icon_weapon_bow', 'Icons/Weapons/bow', 'weapon', 'bow', 64, 2, 'Лук', 'Bow', 1),
(39, 'icon_weapon_staff', 'Icons/Weapons/staff', 'weapon', 'staff', 64, 2, 'Посох', 'Staff', 1),
(40, 'icon_weapon_wand', 'Icons/Weapons/wand', 'weapon', 'wand', 64, 2, 'Жезл', 'Wand', 1);

-- Броня
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(41, 'icon_armor_linen', 'Icons/Armor/linen', 'armor', 'cloth', 64, 1, 'Льняная броня', 'Linen Armor', 1),
(42, 'icon_armor_silk', 'Icons/Armor/silk', 'armor', 'cloth', 64, 2, 'Шёлковая броня', 'Silk Armor', 1),
(43, 'icon_armor_magic', 'Icons/Armor/magic', 'armor', 'cloth', 64, 3, 'Магическая броня', 'Magic Armor', 1),
(44, 'icon_armor_epic', 'Icons/Armor/epic', 'armor', 'cloth', 64, 4, 'Эпическая броня', 'Epic Armor', 1),
(45, 'icon_armor_plate', 'Icons/Armor/plate', 'armor', 'plate', 64, 3, 'Латная броня', 'Plate Armor', 1),
(46, 'icon_armor_leather', 'Icons/Armor/leather', 'armor', 'leather', 64, 2, 'Кожаная броня', 'Leather Armor', 1),
(47, 'icon_armor_mail', 'Icons/Armor/mail', 'armor', 'mail', 64, 2, 'Кольчужная броня', 'Mail Armor', 1);

-- Расходники
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(48, 'icon_potion_health_small', 'Icons/Consumable/health_small', 'consumable', 'potion', 64, 1, 'Малое зелье исцеления', 'Small Healing Potion', 1),
(49, 'icon_potion_health_medium', 'Icons/Consumable/health_medium', 'consumable', 'potion', 64, 2, 'Среднее зелье исцеления', 'Medium Healing Potion', 1),
(50, 'icon_potion_health_large', 'Icons/Consumable/health_large', 'consumable', 'potion', 64, 3, 'Большое зелье исцеления', 'Large Healing Potion', 1),
(51, 'icon_potion_mana_small', 'Icons/Consumable/mana_small', 'consumable', 'potion', 64, 1, 'Малое зелье маны', 'Small Mana Potion', 1),
(52, 'icon_potion_mana_medium', 'Icons/Consumable/mana_medium', 'consumable', 'potion', 64, 2, 'Среднее зелье маны', 'Medium Mana Potion', 1),
(53, 'icon_potion_mana_large', 'Icons/Consumable/mana_large', 'consumable', 'potion', 64, 3, 'Большое зелье маны', 'Large Mana Potion', 1),
(54, 'icon_potion_strength', 'Icons/Consumable/strength', 'consumable', 'potion', 64, 2, 'Зелье силы', 'Strength Potion', 1),
(55, 'icon_potion_agility', 'Icons/Consumable/agility', 'consumable', 'potion', 64, 2, 'Зелье ловкости', 'Agility Potion', 1),
(56, 'icon_potion_intellect', 'Icons/Consumable/intellect', 'consumable', 'potion', 64, 2, 'Зелье интеллекта', 'Intellect Potion', 1),
(57, 'icon_elixir_immortality', 'Icons/Consumable/elixir_immortality', 'consumable', 'elixir', 64, 3, 'Эликсир бессмертия', 'Elixir of Immortality', 1),
(58, 'icon_elixir_absorption', 'Icons/Consumable/elixir_absorption', 'consumable', 'elixir', 64, 3, 'Эликсир поглощения', 'Elixir of Absorption', 1);

-- Аксессуары
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(59, 'icon_ring_strength', 'Icons/Accessories/ring_strength', 'item', 'ring', 64, 2, 'Кольцо силы', 'Ring of Strength', 1),
(60, 'icon_ring_agility', 'Icons/Accessories/ring_agility', 'item', 'ring', 64, 2, 'Кольцо ловкости', 'Ring of Agility', 1),
(61, 'icon_ring_intellect', 'Icons/Accessories/ring_intellect', 'item', 'ring', 64, 2, 'Кольцо интеллекта', 'Ring of Intellect', 1),
(62, 'icon_ring_power', 'Icons/Accessories/ring_power', 'item', 'ring', 64, 3, 'Кольцо мощи', 'Ring of Power', 1),
(63, 'icon_amulet_vitality', 'Icons/Accessories/amulet_vitality', 'item', 'amulet', 64, 2, 'Амулет выносливости', 'Amulet of Vitality', 1),
(64, 'icon_amulet_spirit', 'Icons/Accessories/amulet_spirit', 'item', 'amulet', 64, 2, 'Амулет духа', 'Amulet of Spirit', 1),
(65, 'icon_amulet_immortality', 'Icons/Accessories/amulet_immortality', 'item', 'amulet', 64, 4, 'Амулет бессмертия', 'Amulet of Immortality', 1);

-- Инженерия
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(66, 'icon_gadget_defibrillator', 'Icons/Engineering/defibrillator', 'item', 'gadget', 64, 2, 'Дефибриллятор', 'Defibrillator', 1),
(67, 'icon_gadget_explosive', 'Icons/Engineering/explosive', 'item', 'gadget', 64, 2, 'Взрывчатка', 'Explosive', 1),
(68, 'icon_gadget_invisibility', 'Icons/Engineering/invisibility', 'item', 'gadget', 64, 3, 'Гаджет невидимости', 'Invisibility Gadget', 1),
(69, 'icon_gadget_rocket_pack', 'Icons/Engineering/rocket_pack', 'item', 'gadget', 64, 3, 'Ракетный ранец', 'Rocket Pack', 1),
(70, 'icon_gadget_turret', 'Icons/Engineering/turret', 'item', 'gadget', 64, 4, 'Турель', 'Turret', 1);

-- Миск
INSERT IGNORE INTO extended_icon_def (id, name, file_path, category, subcategory, size_px, quality, description_ru, description_en, is_active) VALUES
(71, 'icon_quest_marker', 'Icons/Misc/quest_marker', 'misc', 'quest', 32, 0, 'Маркер квеста', 'Quest Marker', 1),
(72, 'icon_quest_complete', 'Icons/Misc/quest_complete', 'misc', 'quest', 32, 0, 'Квест выполнен', 'Quest Complete', 1),
(73, 'icon_gold', 'Icons/Misc/gold', 'misc', 'currency', 32, 0, 'Золото', 'Gold', 1),
(74, 'icon_silver', 'Icons/Misc/silver', 'misc', 'currency', 32, 0, 'Серебро', 'Silver', 1),
(75, 'icon_copper', 'Icons/Misc/copper', 'misc', 'currency', 32, 0, 'Медь', 'Copper', 1),
(76, 'icon_reputation', 'Icons/Misc/reputation', 'misc', 'reputation', 32, 0, 'Репутация', 'Reputation', 1),
(77, 'icon_experience', 'Icons/Misc/experience', 'misc', 'xp', 32, 0, 'Опыт', 'Experience', 1),
(78, 'icon_health', 'Icons/Misc/health', 'misc', 'stat', 32, 0, 'Здоровье', 'Health', 1),
(79, 'icon_mana', 'Icons/Misc/mana', 'misc', 'stat', 32, 0, 'Мана', 'Mana', 1);

-- Конфиг
CREATE TABLE IF NOT EXISTS extended_icon_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO extended_icon_config (config_key, config_value, comment) VALUES
('icons_enabled', 'true', 'Включить иконки'),
('default_size', '64', 'Размер по умолчанию'),
('quality_borders', 'true', 'Рамки качества'),
('tooltip_enabled', 'true', 'Тултипы');
(18, 'icon_skill_drain_life', 'Icons/Skills/drain_life', 'skill', 'magical', 64, 0, 'Вытягивание жизни', 'Drain Life', 1);