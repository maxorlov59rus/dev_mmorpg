-- ============================================================================
-- Game_Calendar.sql — Календарь и события
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. CALENDAR_EVENT_DEF — События календаря
-- ---------------------------------------------------------------------------
(11, 'Подземелье Забытых', 'Dungeon of the Forgotten', 'Ежедневный данж для игроков 10+ уровня.', 'Daily dungeon for players 10+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/dungeon_forgotten', '#8B4513', 'spirit_exp', 0, 200, 1),
(12, 'Пещеры Теней', 'Caves of Shadows', 'Ежедневный данж для игроков 15+ уровня.', 'Daily dungeon for players 15+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/dungeon_shadows', '#2F4F4F', 'spirit_exp', 0, 250, 1),
(13, 'Руины Древних', 'Ruins of the Ancients', 'Ежедневный данж для игроков 20+ уровня.', 'Daily dungeon for players 20+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/dungeon_ruins', '#8B0000', 'spirit_exp', 0, 300, 1),
(14, 'Рейд: Цитадель Тьмы', 'Raid: Citadel of Darkness', 'Еженедельный рейд для игроков 30+ уровня.', 'Weekly raid for players 30+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/raid_citadel', '#4B0082', 'spirit_exp', 0, 1000, 1),
(15, 'Рейд: Логово Дракона', 'Raid: Dragon Lair', 'Еженедельный рейд для игроков 40+ уровня.', 'Weekly raid for players 40+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/raid_dragon', '#8B0000', 'spirit_exp', 0, 1500, 1),
(16, 'Инстанс: Разлом', 'Instance: Rift', 'Инстанс для игроков 25+ уровня.', 'Instance for players 25+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/instance_rift', '#4169E1', 'spirit_exp', 0, 400, 1),
(17, 'Инстанс: Астральный План', 'Instance: Astral Plane', 'Инстанс для игроков 35+ уровня.', 'Instance for players 35+.', 4, 1, 1, 12, 31, 1, 'Icons/Events/instance_astral', '#9400D3', 'spirit_exp', 0, 600, 1),
(18, 'Misty Anomaly', 'Misty Anomaly', 'Еженедельный PvE/PvP ивент (воскресенье 20:00).', 'Weekly PvE/PvP event (Sunday 20:00).', 4, 1, 1, 12, 31, 1, 'Icons/Events/misty_anomaly', '#4682B4', 'spirit_exp', 0, 500, 1),
(19, 'PvP: Арена 1v1', 'PvP: Arena 1v1', 'Еженедельная арена 1v1.', 'Weekly Arena 1v1.', 3, 1, 1, 12, 31, 1, 'Icons/Events/arena_1v1', '#FF0000', 'honor', 0, 100, 1),
(20, 'PvP: Арена 2v2', 'PvP: Arena 2v2', 'Еженедельная арена 2v2.', 'Weekly Arena 2v2.', 3, 1, 1, 12, 31, 1, 'Icons/Events/arena_2v2', '#FF4500', 'honor', 0, 200, 1),
(21, 'PvP: Арена 3v3', 'PvP: Arena 3v3', 'Еженедельная арена 3v3.', 'Weekly Arena 3v3.', 3, 1, 1, 12, 31, 1, 'Icons/Events/arena_3v3', '#FF8C00', 'honor', 0, 300, 1),
(22, 'PvP: Поле боя 10v10', 'PvP: Battleground 10v10', 'Еженедельное поле боя.', 'Weekly battleground.', 3, 1, 1, 12, 31, 1, 'Icons/Events/battleground', '#DC143C', 'honor', 0, 400, 1),
(23, 'PvP: Захват флага', 'PvP: Capture the Flag', 'Еженедельный CTF.', 'Weekly CTF.', 3, 1, 1, 12, 31, 1, 'Icons/Events/ctf', '#00CED1', 'honor', 0, 350, 1),
(24, 'PvP: Осада Цитадели', 'PvP: Citadel Siege', 'Еженедельная осада (гильдии).', 'Weekly guild siege.', 3, 1, 1, 12, 31, 1, 'Icons/Events/siege', '#8B0000', 'honor', 0, 500, 1),
(25, 'PvP: Фронт', 'PvP: Frontline', 'Еженедельный фронт (фракции).', 'Weekly faction frontline.', 3, 1, 1, 12, 31, 1, 'Icons/Events/frontline', '#B22222', 'honor', 0, 450, 1),
(26, 'PvP: Пороговый Караван', 'PvP: Threshold Caravan', 'Еженедельный караван (смешанный).', 'Weekly mixed caravan.', 3, 1, 1, 12, 31, 1, 'Icons/Events/caravan', '#D2691E', 'honor', 0, 300, 1),
(27, 'PvP: Дуэльный турнир', 'PvP: Duel Tournament', 'Еженедельный турнир дуэлей.', 'Weekly duel tournament.', 3, 1, 1, 12, 31, 1, 'Icons/Events/duel', '#FFD700', 'honor', 0, 600, 1),
(28, 'PvP: Королевская битва', 'PvP: Battle Royale', 'Еженедельная королевская битва (100 игроков).', 'Weekly battle royale (100 players).', 3, 1, 1, 12, 31, 1, 'Icons/Events/battle_royale', '#8B008B', 'honor', 0, 1000, 1),
(29, 'Сезонное: Весна', 'Seasonal: Spring', 'Весеннее событие (март-май).', 'Spring event (March-May).', 2, 3, 1, 5, 31, 1, 'Icons/Events/spring', '#00FF00', 'item', 0, 0, 1),
(30, 'Сезонное: Лето', 'Seasonal: Summer', 'Летнее событие (июнь-август).', 'Summer event (June-August).', 2, 6, 1, 8, 31, 1, 'Icons/Events/summer', '#FFD700', 'item', 0, 0, 1),
(31, 'Сезонное: Осень', 'Seasonal: Autumn', 'Осенее событие (сентябрь-ноябрь).', 'Autumn event (September-November).', 2, 9, 1, 11, 30, 1, 'Icons/Events/autumn', '#FF8C00', 'item', 0, 0, 1),
(32, 'Сезонное: Зима', 'Seasonal: Winter', 'Зимнее событие (декабрь-февраль).', 'Winter event (December-February).', 2, 12, 1, 2, 28, 1, 'Icons/Events/winter', '#00BFFF', 'item', 0, 0, 1);
CREATE TABLE IF NOT EXISTS calendar_event_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT,
    description_en TEXT,
    event_type TINYINT UNSIGNED NOT NULL DEFAULT 1 COMMENT '1=holiday, 2=seasonal, 3=pvp, 4=pve, 5=maintenance',
    start_month TINYINT UNSIGNED NOT NULL,
    start_day TINYINT UNSIGNED NOT NULL,
    end_month TINYINT UNSIGNED NOT NULL,
    end_day TINYINT UNSIGNED NOT NULL,
    is_recurring TINYINT UNSIGNED NOT NULL DEFAULT 1,
    icon_path VARCHAR(128),
    color_hex VARCHAR(8) NOT NULL DEFAULT '#FFFFFF',
    reward_type VARCHAR(32),
    reward_id INT UNSIGNED,
    reward_count INT UNSIGNED DEFAULT 0,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO calendar_event_def (id, name_ru, name_en, description_ru, description_en, event_type, start_month, start_day, end_month, end_day, is_recurring, icon_path, color_hex, reward_type, reward_id, reward_count, is_active) VALUES
(1, 'Новый Год', 'New Year', 'Празднование Нового Года. Подарки и фейерверки!', 'New Year celebration. Gifts and fireworks!', 1, 1, 1, 1, 7, 1, 'Icons/Events/new_year', '#FF0000', 'item', 0, 0, 1),
(2, 'День Влюблённых', 'Valentine Day', 'День любви и романтики.', 'Day of love and romance.', 1, 2, 14, 2, 14, 1, 'Icons/Events/valentine', '#FF69B4', 'title', 9010, 1, 1),
(3, 'Масленица', 'Maslenitsa', 'Праздник проводов зимы.', 'Farewell to winter festival.', 1, 2, 20, 3, 3, 1, 'Icons/Events/maslenitsa', '#FFA500', 'item', 0, 0, 1),
(4, 'День Победы', 'Victory Day', 'Праздник победы.', 'Victory Day celebration.', 1, 5, 9, 5, 9, 1, 'Icons/Events/victory_day', '#008000', 'title', 9011, 1, 1),
(5, 'Летнее Солнцестояние', 'Summer Solstice', 'Самый длинный день в году.', 'Longest day of the year.', 2, 6, 20, 6, 22, 1, 'Icons/Events/summer_solstice', '#FFD700', 'item', 0, 0, 1),
(6, 'Осенний Фестиваль', 'Autumn Festival', 'Праздник урожая.', 'Harvest festival.', 4, 9, 15, 9, 30, 1, 'Icons/Events/autumn_festival', '#8B4513', 'item', 0, 0, 1),
(7, 'Хэллоуин', 'Halloween', 'Ночь духов и монстров.', 'Night of spirits and monsters.', 1, 10, 28, 11, 2, 1, 'Icons/Events/halloween', '#800080', 'title', 9012, 1, 1),
(8, 'Зимнее Солнцестояние', 'Winter Solstice', 'Самый короткий день в году.', 'Shortest day of the year.', 2, 12, 20, 12, 22, 1, 'Icons/Events/winter_solstice', '#000080', 'item', 0, 0, 1),
(9, 'Битва Духов (еженедельно)', 'Spirit Battle (Weekly)', 'Еженедельный PvP ивент Битва Духов.', 'Weekly PvP event Spirit Battle.', 3, 1, 1, 12, 31, 1, 'Icons/Events/spirit_battle', '#FF4500', 'spirit_exp', 0, 1500, 1),
(10, 'Туманная Аномалия (еженедельно)', 'Misty Anomaly (Weekly)', 'Еженедельный PvE/PvP ивент.', 'Weekly PvE/PvP event.', 4, 1, 1, 12, 31, 1, 'Icons/Events/misty_anomaly', '#4682B4', 'spirit_exp', 0, 500, 1);

-- ---------------------------------------------------------------------------
-- 2. CALENDAR_CONFIG — Конфигурация календаря
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS calendar_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO calendar_config (config_key, config_value, comment) VALUES
('calendar_enabled', 'true', 'Включить календарь'),
('show_events', 'true', 'Показывать события'),
('show_weekends', 'true', 'Подсвечивать выходные'),
('first_day_of_week', '1', 'Первый день недели (0=Вс, 1=Пн)'),
('date_format', 'DD.MM.YYYY', 'Формат даты'),
('time_format', '24h', 'Формат времени (12h/24h)');

-- ---------------------------------------------------------------------------
-- 3. CALENDAR_EVENT_LOG — Лог участия в событиях
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS calendar_event_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    event_id INT UNSIGNED NOT NULL,
    participated_at INT UNSIGNED NOT NULL,
    reward_claimed TINYINT UNSIGNED NOT NULL DEFAULT 0,
    INDEX idx_player (player_guid),
    INDEX idx_event (event_id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 4. NPC — Хранитель Календаря
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(96001, 'Хранитель Календаря', 'Летописец Времени', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(96001, 1, 65536*140, 65536*95, 65536*10, 0x5);