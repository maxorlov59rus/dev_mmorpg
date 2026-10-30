-- ============================================================================
-- Seasonal_Events.sql — Сезонные эвенты по временам года
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. SEASONAL_EVENT_DEF — Определение 4 сезонных эвентов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_event_def (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    season TINYINT UNSIGNED NOT NULL,
    start_month TINYINT UNSIGNED NOT NULL,
    start_day TINYINT UNSIGNED NOT NULL,
    end_month TINYINT UNSIGNED NOT NULL,
    end_day TINYINT UNSIGNED NOT NULL,
    currency_type TINYINT UNSIGNED NOT NULL,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_event_def (id, name_ru, name_en, season, start_month, start_day, end_month, end_day, currency_type, is_active, comment) VALUES
(1, 'Праздник Рассвета', 'Dawn Festival', 1, 3, 20, 4, 20, 1, 1, 'Весна: 20 марта - 20 апреля'),
(2, 'Праздник Поглощения', 'Hollow Festival', 2, 6, 21, 7, 21, 2, 1, 'Лето: 21 июня - 21 июля'),
(3, 'Праздник Порога', 'Threshold Festival', 3, 9, 23, 10, 23, 3, 1, 'Осень: 23 сентября - 23 октября'),
(4, 'Праздник Тьмы', 'Darkness Festival', 4, 12, 21, 1, 21, 4, 1, 'Зима: 21 декабря - 21 января');

-- ---------------------------------------------------------------------------
-- 2. SEASONAL_CURRENCY — Сезонная валюта
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_currency (
    id TINYINT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    season_id TINYINT UNSIGNED NOT NULL,
    icon VARCHAR(64) NOT NULL,
    FOREIGN KEY (season_id) REFERENCES seasonal_event_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_currency (id, name_ru, name_en, season_id, icon) VALUES
(1, 'Лепестки Рассвета', 'Dawn Petals', 1, 'icon_dawn_petals'),
(2, 'Угли Поглощения', 'Hollow Embers', 2, 'icon_hollow_embers'),
(3, 'Золотые Листья', 'Golden Leaves', 3, 'icon_golden_leaves'),
(4, 'Снежинки Тьмы', 'Dark Snowflakes', 4, 'icon_dark_snowflakes');

-- ---------------------------------------------------------------------------
-- 3. SEASONAL_NPC — NPC сезонных эвентов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_npc (
    entry INT UNSIGNED NOT NULL PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    season_id TINYINT UNSIGNED NOT NULL,
    role VARCHAR(32) NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    FOREIGN KEY (season_id) REFERENCES seasonal_event_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_npc (entry, name_ru, name_en, season_id, role, pos_x, pos_y, pos_z, map_id) VALUES
(90020, 'Весенний вестник', 'Spring Herald', 1, 'event_host', 200.0, 100.0, 10.0, 1),
(90021, 'Торговец Рассвета', 'Dawn Vendor', 1, 'vendor', 210.0, 105.0, 10.0, 1),
(90022, 'Садовник Рассвета', 'Dawn Gardener', 1, 'quest_giver', 195.0, 95.0, 10.0, 1),
(90030, 'Летний вестник', 'Summer Herald', 2, 'event_host', 300.0, 150.0, 15.0, 2),
(90031, 'Торговец Круга', 'Hollow Vendor', 2, 'vendor', 310.0, 155.0, 15.0, 2),
-- ---------------------------------------------------------------------------
-- ---------------------------------------------------------------------------
-- 5. SEASONAL_SHOP — Магазины сезонных товаров
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_shop_item (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    season_id TINYINT UNSIGNED NOT NULL,
    item_entry INT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT,
    description_en TEXT,
    currency_cost INT UNSIGNED NOT NULL,
    required_level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_available TINYINT UNSIGNED NOT NULL DEFAULT 1,
    stock INT UNSIGNED NOT NULL DEFAULT -1,
    FOREIGN KEY (season_id) REFERENCES seasonal_event_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_shop_item (id, season_id, item_entry, name_ru, name_en, description_ru, description_en, currency_cost, required_level, is_available, stock) VALUES
(1, 1, 16001, 'Венок Рассвета', 'Dawn Wreath', 'Праздничный венок из весенних цветов.', 'Festival wreath of spring flowers.', 50, 1, 1, -1),
(2, 1, 16002, 'Фонарь Рассвета', 'Dawn Lantern', 'Светящийся фонарь для праздника.', 'Glowing lantern for the festival.', 75, 5, 1, -1),
(3, 1, 16003, 'Песнь Рассвета', 'Dawn Song', 'Музыкальный свиток с песней рассвета.', 'Musical scroll with dawn song.', 100, 10, 1, -1),
(4, 1, 16013, 'Крылья Рассвета', 'Dawn Wings', 'Косметические крылья рассвета.', 'Cosmetic dawn wings.', 500, 20, 1, -1),
(5, 2, 16004, 'Амулет Поглощения', 'Hollow Amulet', 'Амулет из углей поглощения.', 'Amulet of hollow embers.', 50, 1, 1, -1),
(6, 2, 16005, 'Теневой плащ', 'Shadow Cloak', 'Плащ из теней.', 'Cloak of shadows.', 75, 5, 1, -1),
(7, 2, 16006, 'Ритуальный кинжал', 'Ritual Dagger', 'Кинжал для ритуалов поглощения.', 'Dagger for hollow rituals.', 100, 10, 1, -1),
(8, 2, 16014, 'Крылья Поглощения', 'Hollow Wings', 'Косметические крылья поглощения.', 'Cosmetic hollow wings.', 500, 20, 1, -1),
(9, 3, 16007, 'Маска Порога', 'Threshold Mask', 'Праздничная маска.', 'Festival mask.', 50, 1, 1, -1),
(10, 3, 16008, 'Торговый жетон', 'Trade Token', 'Жетон для торговли на ярмарке.', 'Token for fair trade.', 75, 5, 1, -1),
(11, 3, 16009, 'Золотой лист', 'Golden Leaf', 'Символ осеннего фестиваля.', 'Symbol of autumn festival.', 100, 10, 1, -1),
(12, 3, 16015, 'Крылья Порога', 'Threshold Wings', 'Косметические крылья порога.', 'Cosmetic threshold wings.', 500, 20, 1, -1),
(13, 4, 16010, 'Снежинка Тьмы', 'Dark Snowflake', 'Кристалл из тёмного снега.', 'Crystal of dark snow.', 50, 1, 1, -1),
(14, 4, 16011, 'Огонь Тьмы', 'Dark Fire', 'Вечный огонь тьмы.', 'Eternal dark fire.', 75, 5, 1, -1),
(15, 4, 16012, 'Воспоминание', 'Memory', 'Воспоминание о павших.', 'Memory of the fallen.', 100, 10, 1, -1),
(16, 4, 16016, 'Крылья Тьмы', 'Dark Wings', 'Косметические крылья тьмы.', 'Cosmetic dark wings.', 500, 20, 1, -1);

-- ---------------------------------------------------------------------------
-- 6. SEASONAL_LOCATIONS — Локации сезонных эвентов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_location (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    season_id TINYINT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    map_id SMALLINT UNSIGNED NOT NULL,
    pos_x FLOAT NOT NULL,
    pos_y FLOAT NOT NULL,
    pos_z FLOAT NOT NULL,
    FOREIGN KEY (season_id) REFERENCES seasonal_event_def(id)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_location (season_id, name_ru, name_en, map_id, pos_x, pos_y, pos_z) VALUES
(1, 'Сад Рассвета', 'Dawn Garden', 1, 200.0, 100.0, 10.0),
(2, 'Очаг Поглощения', 'Hollow Hearth', 2, 300.0, 150.0, 15.0),
(3, 'Ярмарка Порога', 'Threshold Fair', 3, 400.0, 200.0, 20.0),
(4, 'Костёр Тьмы', 'Dark Bonfire', 4, 500.0, 250.0, 25.0);

-- ---------------------------------------------------------------------------
-- 7. SEASONAL_CONFIG — Конфигурация сезонных эвентов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_config (config_key, config_value, comment) VALUES
('seasonal_events_enabled', 'true', 'Включить сезонные эвенты'),
('auto_start', 'true', 'Автоматический старт по расписанию'),
('notification_hours_before', '24', 'Уведомление за N часов до начала'),
('max_participants', '1000', 'Макс участников эвента'),
('reward_multiplier', '1.0', 'Множитель наград'),
('seasonal_currency_cap', '10000', 'Лимит сезонной валюты'),
('event_duration_days', '30', 'Длительность эвента (дни)');
-- 4. SEASONAL_QUEST — Сезонные квесты
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS seasonal_quest (
    id INT UNSIGNED NOT NULL PRIMARY KEY,
    season_id TINYINT UNSIGNED NOT NULL,
    title_ru VARCHAR(128) NOT NULL,
    title_en VARCHAR(128) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    objective_ru VARCHAR(255) NOT NULL,
    objective_en VARCHAR(255) NOT NULL,
    npc_entry INT UNSIGNED NOT NULL,
    reward_currency_amount INT UNSIGNED NOT NULL DEFAULT 10,
    reward_item_id INT UNSIGNED NOT NULL DEFAULT 0,
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 10,
    FOREIGN KEY (season_id) REFERENCES seasonal_event_def(id),
    FOREIGN KEY (npc_entry) REFERENCES seasonal_npc(entry)
) ENGINE=InnoDB;

INSERT IGNORE INTO seasonal_quest (id, season_id, title_ru, title_en, description_ru, description_en, objective_ru, objective_en, npc_entry, reward_currency_amount, reward_item_id, min_level) VALUES
(1, 1, 'Цветение Рассвета', 'Dawn Blooming', 'Собери весенние цветы для праздника.', 'Gather spring flowers for the festival.', 'Собрать 10 весенних цветов', 'Gather 10 spring flowers', 90022, 20, 16001, 10),
(2, 1, 'Свет Рассвета', 'Dawn Light', 'Зажги фонари в столице.', 'Light lanterns in the capital.', 'Зажечь 5 фонарей', 'Light 5 lanterns', 90022, 15, 16002, 10),
(3, 1, 'Песнь Рассвета', 'Dawn Song', 'Спой песнь рассвета у алтаря.', 'Sing the dawn song at the altar.', 'Спеть песнь рассвета', 'Sing the dawn song', 90020, 25, 16003, 15),
(4, 2, 'Жар Поглощения', 'Hollow Heat', 'Собери угли из очагов.', 'Gather embers from hearths.', 'Собрать 10 углей', 'Gather 10 embers', 90032, 20, 16004, 10),
(5, 2, 'Тень Поглощения', 'Hollow Shadow', 'Пройди через теневой лабиринт.', 'Navigate the shadow maze.', 'Пройти теневой лабиринт', 'Navigate shadow maze', 90032, 25, 16005, 15),
(6, 2, 'Ритуал Поглощения', 'Hollow Ritual', 'Участвуй в ритуале Поглощения.', 'Participate in the Hollow ritual.', 'Участвовать в ритуале', 'Participate in ritual', 90030, 30, 16006, 20),
(7, 3, 'Урожай Порога', 'Threshold Harvest', 'Собери урожай для фестиваля.', 'Gather harvest for the festival.', 'Собрать 10 овощей', 'Gather 10 vegetables', 90042, 20, 16007, 10),
(8, 3, 'Маска Порога', 'Threshold Mask', 'Создай праздничную маску.', 'Craft a festival mask.', 'Создать маску', 'Craft a mask', 90042, 25, 16008, 15),
(9, 3, 'Торговля Порога', 'Threshold Trade', 'Продай товары на ярмарке.', 'Sell goods at the fair.', 'Продать 5 товаров', 'Sell 5 goods', 90040, 30, 16009, 20),
(10, 4, 'Снег Тьмы', 'Dark Snow', 'Собери снежинки тьмы.', 'Gather dark snowflakes.', 'Собрать 10 снежинок', 'Gather 10 snowflakes', 90052, 20, 16010, 10),
(11, 4, 'Огонь Тьмы', 'Dark Fire', 'Зажги костры в столице.', 'Light fires in the capital.', 'Зажечь 5 костров', 'Light 5 fires', 90052, 25, 16011, 15),
(12, 4, 'Воспоминания Тьмы', 'Dark Memories', 'Почти память о павших.', 'Honor the fallen.', 'Почтить память 5 павших', 'Honor 5 fallen', 90050, 30, 16012, 20);
(90032, 'Жрец Поглощения', 'Hollow Priest', 2, 'quest_giver', 295.0, 145.0, 15.0, 2),
(90040, 'Осенний вестник', 'Autumn Herald', 3, 'event_host', 400.0, 200.0, 20.0, 3),
(90041, 'Торговец Порога', 'Threshold Vendor', 3, 'vendor', 410.0, 205.0, 20.0, 3),
(90042, 'Торговец масками', 'Mask Merchant', 3, 'quest_giver', 395.0, 195.0, 20.0, 3),
(90050, 'Зимний вестник', 'Winter Herald', 4, 'event_host', 500.0, 250.0, 25.0, 4),
(90051, 'Торговец зимы', 'Winter Vendor', 4, 'vendor', 510.0, 255.0, 25.0, 4),
(90052, 'Хранитель огня', 'Fire Keeper', 4, 'quest_giver', 495.0, 245.0, 25.0, 4);