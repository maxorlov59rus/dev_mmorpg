-- ============================================================================
-- Resurrection_System.sql — Система воскрешения: кладбища, духовные целители, заклинания
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. GRAVEYARD — Локации кладбищ
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS graveyard (
    id INT UNSIGNED NOT NULL PRIMARY KEY,
    map_id SMALLINT UNSIGNED NOT NULL,
    zone_id TINYINT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    is_instance TINYINT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO graveyard (id, map_id, zone_id, name, pos_x, pos_y, pos_z, is_instance, comment) VALUES
(1,  1, 2, 'Кладбище Леса Испытаний',   120.5, 45.2, 12.0, 0, 'Стартовая зона'),
(2,  2, 4, 'Кладбище Ржавых пустошей',   340.0, 120.0, 8.5, 0, 'Зона 10-15'),
(3,  3, 5, 'Кладбище Крепости Пепла',   500.0, 200.0, 15.0, 0, 'Хаб фракции Альянса'),
(4,  5, 6, 'Кладбище Тёмных предгорий',  150.0, 80.0, 20.0, 0, 'Зона 16-20'),
(5,  6, 7, 'Кладбище Храма забвения',    300.0, 150.0, 25.0, 0, 'Зона 19-22'),
(6,  7, 8, 'Кладбище Аванпоста демонов', 450.0, 220.0, 30.0, 0, 'Зона 21-24'),
(7,  8, 9, 'Кладбище Цитадели тьмы',    600.0, 300.0, 35.0, 0, 'Зона 23-26'),
(8,  9, 10, 'Кладбище Базы героев',     750.0, 380.0, 40.0, 0, 'Зона 25-30'),
(100, 20, 20, 'Кладбище Пещер Испытаний', 50.0, 50.0, 5.0, 1, 'Инстанс map 20'),
(101, 7,  7,  'Кладбище Храма забвения',  80.0, 80.0, 10.0, 1, 'Инстанс map 7'),
(102, 9,  9,  'Кладбище Цитадели тьмы',  100.0, 100.0, 15.0, 1, 'Инстанс map 9');

-- ---------------------------------------------------------------------------
-- 2. SPIRIT_HEALER — NPC духовные целители
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS spirit_healer (
    entry INT UNSIGNED NOT NULL PRIMARY KEY,
    graveyard_id INT UNSIGNED NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    name VARCHAR(64) NOT NULL,
    subname VARCHAR(64),
-- ---------------------------------------------------------------------------
-- 3. RESURRECTION_SPELLS — Заклинания воскрешения
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS resurrection_spell (
    spell_id INT UNSIGNED NOT NULL PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    type TINYINT UNSIGNED NOT NULL,
    cast_time_ms INT UNSIGNED NOT NULL DEFAULT 0,
    cooldown_ms INT UNSIGNED NOT NULL DEFAULT 0,
    mana_cost INT UNSIGNED NOT NULL DEFAULT 0,
    health_restored_pct TINYINT UNSIGNED NOT NULL DEFAULT 50,
    has_durability_loss TINYINT UNSIGNED NOT NULL DEFAULT 1,
    has_sickness TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sickness_spell_id INT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO resurrection_spell (spell_id, name, type, cast_time_ms, cooldown_ms, mana_cost, health_restored_pct, has_durability_loss, has_sickness, sickness_spell_id, comment) VALUES
(90001, 'Воскрешение', 1, 10000, 0, 100, 50, 1, 0, 0, 'Базовое воскрешение союзником'),
(90002, 'Массовое воскрешение', 1, 10000, 0, 200, 50, 1, 0, 0, 'Воскрешение группы'),
(90003, 'Камень души', 2, 0, 0, 0, 100, 0, 0, 0, 'Самовоскрешение (Чернокнижник)'),
(90004, 'Воскрешение инженера', 2, 0, 0, 0, 50, 1, 0, 0, 'Самовоскрешение (Инженерия)'),
(90005, 'Духовное воскрешение', 3, 0, 0, 0, 50, 1, 1, 90010, 'Воскрешение у духовного целителя'),
(90010, 'Болезнь воскрешения', 0, 0, 0, 0, 0, 0, 0, 0, 'Дебафф после воскрешения у целителя');

-- ---------------------------------------------------------------------------
-- 4. RESURRECTION_ITEMS — Предметы самовоскрешения
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS resurrection_item (
    item_id INT UNSIGNED NOT NULL PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    spell_id INT UNSIGNED NOT NULL,
    cooldown_ms INT UNSIGNED NOT NULL DEFAULT 0,
    charges INT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255),
    FOREIGN KEY (spell_id) REFERENCES resurrection_spell(spell_id)
) ENGINE=InnoDB;

INSERT IGNORE INTO resurrection_item (item_id, name, spell_id, cooldown_ms, charges, comment) VALUES
(90101, 'Камень души (малый)', 90003, 0, 1, 'Самовоскрешение, 1 заряд'),
(90102, 'Камень души (большой)', 90003, 0, 3, 'Самовоскрешение, 3 заряда'),
(90103, 'Дефибриллятор инженера', 90004, 300000, 10, 'Самовоскрешение инженера, 10 зарядов, КД 5 мин'),
(90104, 'Анкх шамана', 90003, 0, 1, 'Самовоскрешение шамана, 1 заряд, восстанавливает 100% HP');

-- ---------------------------------------------------------------------------
-- 5. INSTANCE_RESURRECTION — Настройки воскрешения в инстансах
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS instance_resurrection (
    map_id SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
    graveyard_id INT UNSIGNED NOT NULL,
    allow_spirit_healer TINYINT UNSIGNED NOT NULL DEFAULT 1,
    allow_corpse_run TINYINT UNSIGNED NOT NULL DEFAULT 0,
    allow_self_res TINYINT UNSIGNED NOT NULL DEFAULT 1,
    allow_ally_res TINYINT UNSIGNED NOT NULL DEFAULT 1,
    res_timer_sec INT UNSIGNED NOT NULL DEFAULT 0,
    comment VARCHAR(255),
    FOREIGN KEY (graveyard_id) REFERENCES graveyard(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO instance_resurrection (map_id, graveyard_id, allow_spirit_healer, allow_corpse_run, allow_self_res, allow_ally_res, res_timer_sec, comment) VALUES
(20, 100, 1, 0, 1, 1, 0, 'Пещеры Испытаний: только духовный целитель и самовоскрешение'),
(7,  101, 1, 0, 1, 1, 0, 'Храм забвения: только духовный целитель и самовоскрешение'),
(9,  102, 1, 0, 1, 1, 0, 'Цитадель тьмы: только духовный целитель и самовоскрешение');
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    faction_alliance INT UNSIGNED NOT NULL DEFAULT 0,
    faction_horde INT UNSIGNED NOT NULL DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    FOREIGN KEY (graveyard_id) REFERENCES graveyard(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO spirit_healer (entry, graveyard_id, map_id, name, subname, pos_x, pos_y, pos_z, faction_alliance, faction_horde, is_active) VALUES
(80001, 1, 1, 'Духовный целитель Аэлия', 'Хранитель душ', 125.0, 50.0, 12.0, 1649, 1650, 1),
(80002, 2, 2, 'Духовный целитель Торн', 'Хранитель душ', 345.0, 125.0, 8.5, 1649, 1650, 1),
(80003, 3, 3, 'Духовный целитель Миранда', 'Хранитель душ', 505.0, 205.0, 15.0, 1649, 1650, 1),
(80004, 4, 5, 'Духовный целитель Кейн', 'Хранитель душ', 155.0, 85.0, 20.0, 1649, 1650, 1),
(80005, 5, 6, 'Духовный целитель Лира', 'Хранитель душ', 305.0, 155.0, 25.0, 1649, 1650, 1),
(80006, 6, 7, 'Духовный целитель Гром', 'Хранитель душ', 455.0, 225.0, 30.0, 1649, 1650, 1),
(80007, 7, 8, 'Духовный целитель Вера', 'Хранитель душ', 605.0, 305.0, 35.0, 1649, 1650, 1),
(80008, 8, 9, 'Духовный целитель Дарк', 'Хранитель душ', 755.0, 385.0, 40.0, 1649, 1650, 1);