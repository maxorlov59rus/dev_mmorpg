-- ============================================================================
-- Crafting_System.sql — Система крафта: профессии, рецепты, ресурсы, трейнеры
-- База: dm_world
-- Профессии доступны с 20 уровня
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 0. TRAINER NPCs — Трейнеры профессий (creature_template)
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(91001, 'Тренер горного дела', 'Горное дело', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91002, 'Тренер травничества', 'Травничество', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91003, 'Тренер снятия шкур', 'Снятие шкур', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91004, 'Тренер кузнечного дела', 'Кузнечное дело', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91005, 'Тренер алхимии', 'Алхимия', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91006, 'Тренер портняжного дела', 'Портняжное дело', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91007, 'Тренер инженерии', 'Инженерия', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0),
(91008, 'Тренер ювелирного дела', 'Ювелирное дело', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

-- ---------------------------------------------------------------------------
-- 1. PROFESSION_DEF — Определение 8 профессий
-- ---------------------------------------------------------------------------

USE dm_world;

CREATE TABLE IF NOT EXISTS profession_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    type TINYINT UNSIGNED NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 20,
    max_skill INT UNSIGNED NOT NULL DEFAULT 300,
    icon VARCHAR(64) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO profession_def (id, name_ru, name_en, type, description_ru, description_en, min_level, max_skill, icon, comment) VALUES
(1, 'Горное дело', 'Mining', 1, 'Добыча руды и камней.', 'Mining ore and stones.', 20, 300, 'icon_mining', 'Собирательство'),
(2, 'Травничество', 'Herbalism', 1, 'Сбор трав и растений.', 'Gathering herbs and plants.', 20, 300, 'icon_herbalism', 'Собирательство'),
(3, 'Снятие шкур', 'Skinning', 1, 'Снятие шкур с животных.', 'Skinning animals.', 20, 300, 'icon_skinning', 'Собирательство'),
(4, 'Кузнечное дело', 'Blacksmithing', 2, 'Ковка оружия и доспехов.', 'Forging weapons and armor.', 20, 300, 'icon_blacksmithing', 'Крафт'),
(5, 'Алхимия', 'Alchemy', 2, 'Создание зелий и эликсиров.', 'Creating potions and elixirs.', 20, 300, 'icon_alchemy', 'Крафт'),
(6, 'Портняжное дело', 'Tailoring', 2, 'Шитьё тканевой брони.', 'Sewing cloth armor.', 20, 300, 'icon_tailoring', 'Крафт'),
(7, 'Инженерия', 'Engineering', 2, 'Создание гаджетов и взрывчатки.', 'Creating gadgets and explosives.', 20, 300, 'icon_engineering', 'Крафт'),
(8, 'Ювелирное дело', 'Jewelcrafting', 2, 'Создание колец и амулетов.', 'Creating rings and amulets.', 20, 300, 'icon_jewelcrafting', 'Крафт');

CREATE TABLE IF NOT EXISTS craft_recipe (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    profession_id TINYINT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    result_item_id INT UNSIGNED NOT NULL,
    result_count INT UNSIGNED NOT NULL DEFAULT 1,
    required_skill INT UNSIGNED NOT NULL DEFAULT 1,
    craft_time_ms INT UNSIGNED NOT NULL DEFAULT 3000,
    description_ru TEXT,
    description_en TEXT,
    FOREIGN KEY (profession_id) REFERENCES profession_def(id)
) ENGINE=InnoDB;

-- Blacksmithing recipes
INSERT IGNORE INTO craft_recipe (id, profession_id, name_ru, name_en, result_item_id, result_count, required_skill, craft_time_ms, description_ru, description_en) VALUES
(4001, 4, 'Железный меч', 'Iron Sword', 14001, 1, 1, 3000, 'Базовый меч для воинов.', 'Basic sword for warriors.'),
(4002, 4, 'Железный щит', 'Iron Shield', 14002, 1, 1, 3000, 'Базовый щит для воинов.', 'Basic shield for warriors.'),
(4003, 4, 'Стальной меч', 'Steel Sword', 14003, 1, 50, 4000, 'Улучшенный меч.', 'Improved sword.'),
(4004, 4, 'Стальной щит', 'Steel Shield', 14004, 1, 50, 4000, 'Улучшенный щит.', 'Improved shield.'),
(4005, 4, 'Мифриловый меч', 'Mithril Sword', 14005, 1, 100, 5000, 'Мощный меч из мифрила.', 'Powerful mithril sword.'),
(4006, 4, 'Мифриловый щит', 'Mithril Shield', 14006, 1, 100, 5000, 'Мощный щит из мифрила.', 'Powerful mithril shield.'),
(4007, 4, 'Титановый меч', 'Titanium Sword', 14007, 1, 200, 6000, 'Легендарный меч из титана.', 'Legendary titanium sword.'),
-- Alchemy recipes
INSERT IGNORE INTO craft_recipe (id, profession_id, name_ru, name_en, result_item_id, result_count, required_skill, craft_time_ms, description_ru, description_en) VALUES
(5001, 5, 'Зелье исцеления (малое)', 'Small Healing Potion', 14010, 1, 1, 2000, 'Восстанавливает 100 HP.', 'Restores 100 HP.'),
(5002, 5, 'Зелье исцеления (среднее)', 'Medium Healing Potion', 14011, 1, 25, 2500, 'Восстанавливает 300 HP.', 'Restores 300 HP.'),
(5003, 5, 'Зелье исцеления (большое)', 'Large Healing Potion', 14012, 1, 75, 3000, 'Восстанавливает 800 HP.', 'Restores 800 HP.'),
(5004, 5, 'Зелье маны (малое)', 'Small Mana Potion', 14013, 1, 1, 2000, 'Восстанавливает 100 маны.', 'Restores 100 mana.'),
(5005, 5, 'Зелье маны (среднее)', 'Medium Mana Potion', 14014, 1, 25, 2500, 'Восстанавливает 300 маны.', 'Restores 300 mana.'),
(5006, 5, 'Зелье маны (большое)', 'Large Mana Potion', 14015, 1, 75, 3000, 'Восстанавливает 800 маны.', 'Restores 800 mana.'),
(5007, 5, 'Зелье силы', 'Strength Potion', 14016, 1, 50, 3000, '+10 силы на 30 мин.', '+10 strength for 30 min.'),
(5008, 5, 'Зелье ловкости', 'Agility Potion', 14017, 1, 50, 3000, '+10 ловкости на 30 мин.', '+10 agility for 30 min.'),
(5009, 5, 'Зелье интеллекта', 'Intellect Potion', 14018, 1, 50, 3000, '+10 интеллекта на 30 мин.', '+10 intellect for 30 min.'),
(5010, 5, 'Зелье выносливости', 'Vitality Potion', 14019, 1, 50, 3000, '+10 выносливости на 30 мин.', '+10 vitality for 30 min.'),
(5011, 5, 'Зелье духа', 'Spirit Potion', 14020, 1, 50, 3000, '+10 духа на 30 мин.', '+10 spirit for 30 min.'),
-- Tailoring recipes
INSERT IGNORE INTO craft_recipe (id, profession_id, name_ru, name_en, result_item_id, result_count, required_skill, craft_time_ms, description_ru, description_en) VALUES
(6001, 6, 'Льняная рубаха', 'Linen Shirt', 14030, 1, 1, 2000, 'Базовая тканевая броня.', 'Basic cloth armor.'),
(6002, 6, 'Шёлковая рубаха', 'Silk Shirt', 14031, 1, 50, 3000, 'Улучшенная тканевая броня.', 'Improved cloth armor.'),
(6003, 6, 'Магическая рубаха', 'Magic Shirt', 14032, 1, 100, 4000, 'Магическая тканевая броня.', 'Magical cloth armor.'),
(6004, 6, 'Эпическая рубаха', 'Epic Shirt', 14033, 1, 200, 5000, 'Эпическая тканевая броня.', 'Epic cloth armor.');

-- Engineering recipes
INSERT IGNORE INTO craft_recipe (id, profession_id, name_ru, name_en, result_item_id, result_count, required_skill, craft_time_ms, description_ru, description_en) VALUES
(7001, 7, 'Дефибриллятор', 'Defibrillator', 14040, 1, 1, 3000, 'Самовоскрешение инженера.', 'Engineer self-resurrection.'),
(7002, 7, 'Взрывчатка', 'Explosive', 14041, 1, 25, 2000, 'Взрывчатка для боя.', 'Combat explosive.'),
(7003, 7, 'Гаджет невидимости', 'Invisibility Gadget', 14042, 1, 50, 4000, 'Временная невидимость.', 'Temporary invisibility.'),
(7004, 7, 'Ракетный ранец', 'Rocket Pack', 14043, 1, 100, 5000, 'Прыжок на 20 метров.', 'Jump 20 meters.'),
(7005, 7, 'Турель', 'Turret', 14044, 1, 150, 6000, 'Автоматическая турель.', 'Automatic turret.');

-- Jewelcrafting recipes
INSERT IGNORE INTO craft_recipe (id, profession_id, name_ru, name_en, result_item_id, result_count, required_skill, craft_time_ms, description_ru, description_en) VALUES
(8001, 8, 'Кольцо силы', 'Ring of Strength', 14050, 1, 1, 2000, 'Кольцо +5 силы.', 'Ring +5 strength.'),
(8002, 8, 'Кольцо ловкости', 'Ring of Agility', 14051, 1, 1, 2000, 'Кольцо +5 ловкости.', 'Ring +5 agility.'),
(8003, 8, 'Кольцо интеллекта', 'Ring of Intellect', 14052, 1, 1, 2000, 'Кольцо +5 интеллекта.', 'Ring +5 intellect.'),
(8004, 8, 'Амулет выносливости', 'Amulet of Vitality', 14053, 1, 25, 3000, 'Амулет +10 выносливости.', 'Amulet +10 vitality.'),
(8005, 8, 'Амулет духа', 'Amulet of Spirit', 14054, 1, 25, 3000, 'Амулет +10 духа.', 'Amulet +10 spirit.'),
(8006, 8, 'Кольцо мощи', 'Ring of Power', 14055, 1, 100, 5000, 'Кольцо +15 ко всем статам.', 'Ring +15 to all stats.'),
(8007, 8, 'Амулет бессмертия', 'Amulet of Immortality', 14056, 1, 200, 6000, 'Амулет +30 выносливости.', 'Amulet +30 vitality.');

(5012, 5, 'Эликсир бессмертия', 'Elixir of Immortality', 14021, 1, 150, 5000, 'Восстанавливает 50% HP при смерти.', 'Restores 50% HP on death.'),
(5013, 5, 'Эликсир поглощения', 'Elixir of Absorption', 14022, 1, 150, 5000, '+20% к сопротивлению тьме на 1 час.', '+20% dark resistance for 1 hour.');

(4008, 4, 'Титановый щит', 'Titanium Shield', 14008, 1, 200, 6000, 'Легендарный щит из титана.', 'Legendary titanium shield.');

CREATE TABLE IF NOT EXISTS craft_recipe_material (
    recipe_id INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    count INT UNSIGNED NOT NULL DEFAULT 1,
    PRIMARY KEY (recipe_id, item_id),
    FOREIGN KEY (recipe_id) REFERENCES craft_recipe(id)
) ENGINE=InnoDB;

-- Blacksmithing materials
INSERT IGNORE INTO craft_recipe_material (recipe_id, item_id, count) VALUES
(4001, 11004, 4), (4001, 11008, 2),
(4002, 11004, 6), (4002, 11008, 3),
(4003, 11004, 6), (4003, 11008, 4),
(4004, 11004, 8), (4004, 11008, 5),
(4005, 11010, 4), (4005, 11008, 2),
(4006, 11010, 6), (4006, 11008, 3),
(4007, 11010, 8), (4007, 11008, 4),
(4008, 11010, 10), (4008, 11008, 5);

-- Alchemy materials
INSERT IGNORE INTO craft_recipe_material (recipe_id, item_id, count) VALUES
(5001, 11007, 2), (5001, 11002, 1),
(5002, 11007, 4), (5002, 11002, 2),
(5003, 11007, 6), (5003, 11009, 2),
(5004, 11007, 2), (5004, 11001, 1),
(5005, 11007, 4), (5005, 11001, 2),
(5006, 11007, 6), (5006, 11009, 2),
(5007, 11003, 2), (5007, 11007, 2),
(5008, 11002, 2), (5008, 11007, 2),
(5009, 11001, 2), (5009, 11007, 2),
(5010, 11004, 2), (5010, 11007, 2),
(5011, 11005, 2), (5011, 11007, 2);

-- Tailoring materials
INSERT IGNORE INTO craft_recipe_material (recipe_id, item_id, count) VALUES
(6001, 11006, 4), (6001, 11007, 2),
(6002, 11006, 6), (6002, 11007, 3),
(6003, 11006, 8), (6003, 11009, 2),
(6004, 11006, 10), (6004, 11009, 4);

-- Engineering materials
INSERT IGNORE INTO craft_recipe_material (recipe_id, item_id, count) VALUES
(7001, 11004, 4), (7001, 11008, 2),
(7002, 11004, 2), (7002, 11003, 1),
(7003, 11006, 3), (7003, 11007, 2),
(7004, 11004, 6), (7004, 11008, 4),
(7005, 11004, 8), (7005, 11008, 6);

-- Jewelcrafting materials
INSERT IGNORE INTO craft_recipe_material (recipe_id, item_id, count) VALUES
(8001, 11004, 2), (8001, 11003, 1),
(8002, 11005, 2), (8002, 11002, 1),
(8003, 11001, 2), (8003, 11007, 1),
(8004, 11004, 3), (8004, 11007, 2),
(8005, 11005, 3), (8005, 11007, 2),
(8006, 11010, 4), (8006, 11008, 2),
(8007, 11010, 6), (8007, 11009, 3);

CREATE TABLE IF NOT EXISTS profession_trainer (
    npc_entry INT UNSIGNED NOT NULL PRIMARY KEY,
    profession_id TINYINT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 20,
    FOREIGN KEY (profession_id) REFERENCES profession_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO profession_trainer (npc_entry, profession_id, name_ru, name_en, pos_x, pos_y, pos_z, map_id, min_level) VALUES
(91001, 1, 'Тренер горного дела', 'Mining Trainer', 200.0, 100.0, 10.0, 1, 20),
(91002, 2, 'Тренер травничества', 'Herbalism Trainer', 210.0, 105.0, 10.0, 1, 20),
(91003, 3, 'Тренер снятия шкур', 'Skinning Trainer', 220.0, 110.0, 10.0, 1, 20),
(91004, 4, 'Тренер кузнечного дела', 'Blacksmithing Trainer', 230.0, 115.0, 10.0, 1, 20),
(91005, 5, 'Тренер алхимии', 'Alchemy Trainer', 240.0, 120.0, 10.0, 1, 20),
(91006, 6, 'Тренер портняжного дела', 'Tailoring Trainer', 250.0, 125.0, 10.0, 1, 20),
(91007, 7, 'Тренер инженерии', 'Engineering Trainer', 260.0, 130.0, 10.0, 1, 20),
(91008, 8, 'Тренер ювелирного дела', 'Jewelcrafting Trainer', 270.0, 135.0, 10.0, 1, 20);

CREATE TABLE IF NOT EXISTS craft_resource_drop (
    mob_entry INT UNSIGNED NOT NULL,
    item_id INT UNSIGNED NOT NULL,
    chance FLOAT NOT NULL DEFAULT 10.0,
    min_count INT UNSIGNED NOT NULL DEFAULT 1,
    max_count INT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255),
    PRIMARY KEY (mob_entry, item_id)
) ENGINE=InnoDB;

INSERT IGNORE INTO craft_resource_drop (mob_entry, item_id, chance, min_count, max_count, comment) VALUES
(50001, 11001, 60.0, 1, 3, 'Ихор слизня'),
(50002, 11002, 40.0, 1, 2, 'Волчья шкура'),
(50002, 11003, 5.0, 1, 1, 'Волчий клык'),
(50003, 11004, 25.0, 1, 2, 'Медный слиток'),
(50004, 11005, 35.0, 1, 2, 'Перо гарпии'),
(50007, 11001, 20.0, 1, 1, 'Ихор слизня'),
(50008, 11006, 30.0, 1, 2, 'Паучий шёлк'),
(50010, 11011, 50.0, 1, 2, 'Теневая шкура'),
(50011, 11012, 40.0, 1, 2, 'Кость нежити'),
(50012, 11013, 30.0, 1, 2, 'Кровь демона'),

-- ---------------------------------------------------------------------------
-- 7. CREATURE_SPAWN — Расстановка трейнеров по миру
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(91001, 1, 65536*200, 65536*100, 65536*10, 0x5),
(91002, 1, 65536*210, 65536*105, 65536*10, 0x5),
(91003, 1, 65536*220, 65536*110, 65536*10, 0x5),
(91004, 1, 65536*230, 65536*115, 65536*10, 0x5),
(91005, 1, 65536*240, 65536*120, 65536*10, 0x5),
(91006, 1, 65536*250, 65536*125, 65536*10, 0x5),
(91007, 1, 65536*260, 65536*130, 65536*10, 0x5),
(91008, 1, 65536*270, 65536*135, 65536*10, 0x5);

(50013, 11014, 100.0, 2, 4, 'Эссенция тьмы'),
(50014, 11015, 100.0, 3, 6, 'Кристалл тьмы'),
(50015, 11016, 45.0, 1, 2, 'Паучий шёлк'),
(50016, 11014, 100.0, 2, 4, 'Эссенция тьмы'),
(50017, 11017, 100.0, 2, 4, 'Каменная пыль'),
(60001, 11030, 100.0, 2, 4, 'Эссенция Разлома'),
(60002, 11031, 100.0, 3, 6, 'Эссенция Искажения'),
(60003, 11032, 100.0, 3, 6, 'Эссенция Некромантии'),
(60004, 11033, 100.0, 4, 8, 'Эссенция Древних'),
(60005, 11034, 100.0, 5, 10, 'Кристалл Чёрной Материи');

CREATE TABLE IF NOT EXISTS profession_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO profession_config (config_key, config_value, comment) VALUES
('professions_enabled', 'true', 'Включить профессии'),
('min_level', '20', 'Минимальный уровень для профессий'),
('max_professions_per_player', '2', 'Макс профессий на игрока'),
('gathering_skill_gain', '1', 'Очки навыка за сбор'),
('craft_skill_gain', '1', 'Очки навыка за крафт'),
('skill_cap_per_level', '5', 'Макс навык = уровень * 5'),
('quality_tiers', 'true', 'Включить тиры качества'),
('invention_gated', 'true', 'Рецепты через открытие знаний');
