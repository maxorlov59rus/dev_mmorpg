-- ============================================================================
-- Bosses_27_50.sql — Боссы квестов 27-50: статы, лут, диалоги, механики
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. CREATURE_TEMPLATE — Боссы (5 основных боссов зон 27-50)
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(60001, 'Страж Разлома', 'Хранитель Разлома Шип', 30, 6, 1650, 1651, 30, 30, 2.0, 5.0, 60001, 0, 100, 200, 1|2|8, 0),
(60002, 'Повелитель Искажённых', 'Владыка Цитадели', 36, 6, 1650, 1651, 36, 36, 3.0, 8.0, 60002, 0, 200, 400, 1|2|8, 0),
(60003, 'Древний Лич', 'Повелитель Мёртвых Пределов', 42, 6, 1650, 1651, 42, 42, 4.0, 10.0, 60003, 0, 300, 600, 1|2|8, 0),
(60004, 'Повелитель Древних', 'Владыка Руин', 46, 6, 1650, 1651, 46, 46, 5.0, 12.0, 60004, 0, 400, 800, 1|2|8, 0),
(60005, 'Фрагмент Чёрной Материи', 'Сознание Пустоты', 50, 6, 1650, 1651, 50, 50, 8.0, 20.0, 60005, 0, 1000, 2000, 1|2|8, 0);

-- ---------------------------------------------------------------------------
-- 2. CREATURE_LOOT_TEMPLATE — Лут боссов
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_loot_template (Entry, Item, Chance, Comment) VALUES
(60001, 13001, 100.0, 'Молот Стража (Rare, 2H Mace)'),
(60001, 13002, 25.0,  'Перстень Разлома (Uncommon)'),
(60001, 11030, 100.0, 'Эссенция Разлома x3'),
(60001, 10860, 15.0,  'Зелье силы (большое)'),
(60002, 13003, 100.0, 'Посох Искажения (Rare, Staff)'),
(60002, 13004, 20.0,  'Доспехи Искажённых (Epic, Chest)'),
(60002, 11031, 100.0, 'Эссенция Искажения x5'),
(60002, 10861, 10.0,  'Зелье маны (большое)'),
(60003, 13005, 100.0, 'Коса Лича (Rare, Polearm)'),
(60003, 13006, 15.0,  'Корона Лича (Epic, Helm)'),
(60003, 11032, 100.0, 'Эссенция Некромантии x5'),
(60003, 10862, 8.0,   'Зелье воскрешения (большое)'),
-- ---------------------------------------------------------------------------
-- ---------------------------------------------------------------------------
-- 6. BOSS_ABILITIES — Способности боссов (детали)
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS boss_ability (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    boss_entry INT UNSIGNED NOT NULL,
    spell_id INT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    damage_type TINYINT UNSIGNED NOT NULL,
    base_damage INT UNSIGNED NOT NULL,
    cooldown_sec INT UNSIGNED NOT NULL,
    chance_pct TINYINT UNSIGNED NOT NULL,
    phase TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_aoe TINYINT UNSIGNED NOT NULL DEFAULT 0,
    radius FLOAT NOT NULL DEFAULT 0,
    comment VARCHAR(255),
    FOREIGN KEY (boss_entry) REFERENCES creature_template(entry)
) ENGINE=InnoDB;

INSERT IGNORE INTO boss_ability (boss_entry, spell_id, name, damage_type, base_damage, cooldown_sec, chance_pct, phase, is_aoe, radius, comment) VALUES
(60001, 6000101, 'Теневой удар', 3, 150, 5, 40, 1, 0, 0, 'Одиночная цель, shadow damage'),
(60001, 6000102, 'Призыв теневых волков', 0, 0, 15, 20, 1, 0, 0, 'Призывает 2 теневых волка'),
(60001, 6000103, 'Усиленный теневой удар', 3, 300, 8, 100, 2, 0, 0, 'Фаза 2: усиленный удар'),
(60002, 6000201, 'Искажение реальности', 1, 200, 8, 35, 1, 1, 10, 'AoE: искажает пространство'),
(60002, 6000202, 'Призыв аддов', 0, 0, 20, 15, 1, 0, 0, 'Призывает 3 адда'),
(60002, 6000203, 'Телепортация', 0, 0, 25, 10, 1, 0, 0, 'Телепортируется к случайной цели'),
(60002, 6000204, 'Массовое искажение', 1, 400, 12, 100, 2, 1, 20, 'Фаза 2: массовое искажение'),
(60003, 6000301, 'Некромантия', 3, 250, 6, 45, 1, 0, 0, 'Shadow damage + heal'),
(60003, 6000302, 'Призыв нежити', 0, 0, 12, 25, 1, 0, 0, 'Призывает 4 нежити'),
(60003, 6000303, 'Ледяная стрела', 5, 180, 10, 30, 1, 0, 0, 'Frost damage'),
(60003, 6000304, 'Массовое воскрешение нежити', 0, 0, 20, 100, 2, 1, 30, 'Фаза 2: воскрешает всех нежить'),
(60004, 6000401, 'Древняя магия', 1, 300, 7, 40, 1, 0, 0, 'Magical damage'),
(60004, 6000402, 'Призыв големов', 0, 0, 18, 20, 1, 0, 0, 'Призывает 2 голема'),
(60004, 6000403, 'Каменный дождь', 0, 150, 15, 25, 1, 1, 15, 'AoE: каменный дождь'),
(60004, 6000404, 'Апокалипсис', 1, 600, 20, 100, 2, 1, 30, 'Фаза 2: апокалипсис'),
(60005, 6000501, 'Поглощение', 3, 400, 5, 50, 1, 0, 0, 'Shadow damage + heal'),
(60005, 6000502, 'Искажение пространства', 1, 250, 10, 30, 1, 1, 20, 'AoE: искажение пространства'),
(60005, 6000503, 'Призыв Полых', 0, 0, 15, 20, 1, 0, 0, 'Призывает 5 Полых'),
(60005, 6000504, 'Абсолютное поглощение', 3, 800, 15, 100, 2, 1, 40, 'Фаза 2: абсолютное поглощение');
-- 3. BOSS_MECHANICS — Механики боссов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS boss_mechanic (
    boss_entry INT UNSIGNED NOT NULL PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    ability_1 VARCHAR(64) NOT NULL,
    ability_1_cd INT UNSIGNED NOT NULL,
    ability_1_chance TINYINT UNSIGNED NOT NULL,
    ability_2 VARCHAR(64),
    ability_2_cd INT UNSIGNED,
    ability_2_chance TINYINT UNSIGNED,
    ability_3 VARCHAR(64),
    ability_3_cd INT UNSIGNED,
    ability_3_chance TINYINT UNSIGNED,
    phase_2_hp_pct TINYINT UNSIGNED NOT NULL DEFAULT 50,
    phase_2_ability VARCHAR(64),
    enrage_timer_sec INT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255),
    FOREIGN KEY (boss_entry) REFERENCES creature_template(entry)
) ENGINE=InnoDB;

INSERT IGNORE INTO boss_mechanic (boss_entry, name, ability_1, ability_1_cd, ability_1_chance, ability_2, ability_2_cd, ability_2_chance, ability_3, ability_3_cd, ability_3_chance, phase_2_hp_pct, phase_2_ability, enrage_timer_sec, comment) VALUES
(60001, 'Страж Разлома', 'Теневой удар', 5, 40, 'Призыв теневых волков', 15, 20, NULL, NULL, NULL, 50, 'Усиленный теневой удар', 300, 'Базовый босс Разлома'),
(60002, 'Повелитель Искажённых', 'Искажение реальности', 8, 35, 'Призыв аддов', 20, 15, 'Телепортация', 25, 10, 40, 'Массовое искажение', 420, 'Босс Цитадели'),
(60003, 'Древний Лич', 'Некромантия', 6, 45, 'Призыв нежити', 12, 25, 'Ледяная стрела', 10, 30, 30, 'Массовое воскрешение нежити', 480, 'Босс Мёртвых Пределов'),
(60004, 'Повелитель Древних', 'Древняя магия', 7, 40, 'Призыв големов', 18, 20, 'Каменный дождь', 15, 25, 25, 'Апокалипсис', 540, 'Босс Руин Древних'),
(60005, 'Фрагмент Чёрной Материи', 'Поглощение', 5, 50, 'Искажение пространства', 10, 30, 'Призыв Полых', 15, 20, 20, 'Абсолютное поглощение', 600, 'Финальный босс Глотки');

-- ---------------------------------------------------------------------------
-- 4. BOSS_DIALOGUE — Диалоги боссов
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO quest_dialogue (quest_id, npc_entry, dialogue_type, locale, text) VALUES
(0, 60001, 1, 'ru', 'Вы осмелились войти в Разлом? Я — Страж, и я не пропущу вас дальше!'),
(0, 60001, 1, 'en', 'You dare enter the Rift? I am the Guardian, and I will not let you pass!'),
(0, 60001, 3, 'ru', 'Невозможно... Как вы...'),
(0, 60001, 3, 'en', 'Impossible... How did you...'),
(0, 60002, 1, 'ru', 'Добро пожаловать в Цитадель Искажённых. Здесь реальность — лишь игрушка.'),
(0, 60002, 1, 'en', 'Welcome to the Citadel of the Distorted. Here reality is but a toy.'),
(0, 60002, 3, 'ru', 'Моя цитадель... падает...'),
(0, 60002, 3, 'en', 'My citadel... is falling...'),
(0, 60003, 1, 'ru', 'Мёртвые Пределы — мой дом. Здесь смерть — лишь начало.'),
(0, 60003, 1, 'en', 'The Dead Limits are my home. Here death is only the beginning.'),
(0, 60003, 3, 'ru', 'Нет... я бессмертен...'),
(0, 60003, 3, 'en', 'No... I am immortal...'),
(0, 60004, 1, 'ru', 'Руины Древних хранят тайны, которые не должны быть раскрыты.'),
(0, 60004, 1, 'en', 'The Ruins of the Ancients hold secrets that must not be revealed.'),
(0, 60004, 3, 'ru', 'Древние... простите меня...'),
(0, 60004, 3, 'en', 'Ancients... forgive me...'),
(0, 60005, 1, 'ru', 'Вы пришли в Глотку. Здесь конец всему. Я — фрагмент сознания Чёрной Материи.'),
(0, 60005, 1, 'en', 'You have come to the Maw. Here is the end of everything. I am a fragment of the Black Matter consciousness.'),
(0, 60005, 3, 'ru', 'Вы... выбрали...'),
(0, 60005, 3, 'en', 'You... chose...');

-- ---------------------------------------------------------------------------
-- 5. BOSS_SPAWN — Локации спавна боссов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS boss_spawn (
    boss_entry INT UNSIGNED NOT NULL PRIMARY KEY,
    map_id SMALLINT UNSIGNED NOT NULL,
    zone_id TINYINT UNSIGNED NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    respawn_time_sec INT UNSIGNED NOT NULL DEFAULT 3600,
    is_instance TINYINT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255),
    FOREIGN KEY (boss_entry) REFERENCES creature_template(entry)
) ENGINE=InnoDB;

INSERT IGNORE INTO boss_spawn (boss_entry, map_id, zone_id, pos_x, pos_y, pos_z, respawn_time_sec, is_instance, comment) VALUES
(60001, 11, 11, 500.0, 250.0, 30.0, 3600, 0, 'Разлом Шип: центральная площадь'),
(60002, 12, 12, 200.0, 100.0, 15.0, 3600, 1, 'Цитадель Искажённых: тронный зал'),
(60003, 13, 13, 400.0, 200.0, 20.0, 3600, 0, 'Мёртвые Пределы: алтарь нежити'),
(60004, 14, 14, 600.0, 300.0, 25.0, 3600, 0, 'Руины Древних: центральный храм'),
(60005, 15, 15, 800.0, 400.0, 35.0, 86400, 1, 'Глотка: финальная арена (рейд)');
(60004, 13007, 100.0, 'Меч Древних (Epic, 1H Sword)'),
(60004, 13008, 25.0,  'Щит Древних (Epic, Shield)'),
(60004, 11033, 100.0, 'Эссенция Древних x8'),
(60004, 10863, 5.0,   'Зелье бессмертия'),
(60005, 13009, 100.0, 'Клинок Пустоты (Legendary, 2H Sword)'),
(60005, 13010, 100.0, 'Доспехи Пустоты (Legendary, Chest)'),
(60005, 13011, 50.0,  'Кольцо Пустоты (Legendary, Ring)'),
(60005, 11034, 100.0, 'Кристалл Чёрной Материи x10'),
(60005, 10864, 100.0, 'Зелье поглощения (Legendary)');