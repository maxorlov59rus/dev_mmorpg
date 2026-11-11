-- 5. PLAYER_MOON_STATE — Состояние Луны для игроков
CREATE TABLE IF NOT EXISTS player_moon_state (
    player_guid BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    current_phase INT UNSIGNED NOT NULL DEFAULT 1,
    last_update INT UNSIGNED NOT NULL DEFAULT 0,
    hp_bonus INT UNSIGNED NOT NULL DEFAULT 0,
    mana_bonus INT UNSIGNED NOT NULL DEFAULT 0,
    crit_bonus INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_player_moon (player_guid)
) ENGINE=InnoDB;

-- 6. PRAYER_COOLDOWN — Коoldown молитв игроков
-- Zodiac System SQL Tables
CREATE TABLE IF NOT EXISTS zodiac_signs (
INSERT IGNORE INTO zodiac_signs (id, name_ru, name_en, element, ruling_planet, elemental_power, compatibility, lucky_number, lucky_color, description_ru, description_en) VALUES
(1, 'Овен', 'Aries', 'fire', 'Mars', 'fire', 'Лев, Стрелец', 1, 'red', 'Огонь signs, полные энергии и решимости', 'Fire signs, full of energy and determination'),
-- 3. MOON_PHASES — Фазы луны
CREATE TABLE IF NOT EXISTS moon_phases (
    id TINYINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    phase_number TINYINT UNSIGNED NOT NULL,
    illumination_percent INT UNSIGNED NOT NULL,
    is_waxing TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_full_moon TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_new_moon TINYINT UNSIGNED NOT NULL DEFAULT 0,
    effect_on_hp_regen TINYINT UNSIGNED NOT NULL DEFAULT 0,
    effect_on_mana_regen TINYINT UNSIGNED NOT NULL DEFAULT 0,
    effect_on_crit_chance TINYINT UNSIGNED NOT NULL DEFAULT 0,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL
) ENGINE=InnoDB;

INSERT IGNORE INTO moon_phases (id, name_ru, name_en, phase_number, illumination_percent, is_waxing, is_full_moon, is_new_moon, effect_on_hp_regen, effect_on_mana_regen, effect_on_crit_chance, description_ru, description_en) VALUES
(1, 'Новолуние', 'New Moon', 1, 0, 0, 1, 1, 0, 0, 0, 'время закладок и новых замыслов', 'New Moon, time for new beginnings'),
(2, 'Первая четверть', 'First Quarter', 2, 50, 1, 0, 0, 5, 2, 1, 'время борьбы и действий', 'First Quarter, time for action and struggle'),
(2, 'Ровящаяся>', 50, 1, 0, 0, 7, 3, 2, 'время усиления', 'Waxing Gibbous, time of strengthening'),
(3, 'Полнолуние', 'Full Moon', 3, 100, 0, 1, 0, 10, 5, 5, 'время максимума энергии', 'Full Moon, time of maximum energy'),
(4, 'Убывающая>', 4, 75, 0, 0, 0, 7, 3, 3, 'время отпускания', 'Waning Gibbous, time of release'),
(4, 'Последняя четверть', 'Last Quarter', 5, 50, 0, 0, 0, 5, 2, 2, 'время анализа и завершения', 'Last Quarter, time of analysis and completion'),
(5, 'Убывающая>', 6, 25, 0, 0, 0, 2, 1, 1, 'время отдыха и рефлексии', 'Waning Crescent, time of rest and reflection'),
(6, 'Новолуние', 'New Moon', 7, 0, 0, 1, 0, 0, 0, 0, 'время закладок и новых замыслов', 'New Moon, time for new beginnings');
"
INSERT IGNORE INTO prayer_definitions (name_ru, name_en, description_ru, description_en, zodiac_sign, min_level, mana_cost, cooldown_minutes, effect_type, effect_value, duration_minutes, is_daily, is_hidden) VALUES
-- Universal prayers
('Молитва за исцеление', ' prayer for healing', 'Молитва за быстрое исцеление', 'Prayer for fast healing', NULL, 1, 10, 60, 'heal', 500, 30, 0, 0),
('Молитва за силу', ' prayer for strength', 'Молитва за выносливость и силу духа', 'Prayer for strength and spirit', NULL, 1, 15, 120, 'buff', 100, 60, 0, 0),
('Молитва за ману', ' prayer for mana', 'Молитва за восстановление маны', 'Prayer for mana restoration', NULL, 1, 10, 60, 'mana', 150, 30, 0, 0),
-- Zodiac-specific prayers
('Молитва Овена', ' prayer of Aries', 'Молитва для Овнов, призывающая воинскую решимость', ' Prayer of Aries, calling forth martial resolve', 1, 1, 5, 30, 'buff', 50, 15, 0, 0),
('Молитва Тельца', ' prayer of Taurus', 'Молитва для Тельцов, призывающая изобилие и стабильность', ' Prayer of Taurus, calling for abundance and stability', 2, 1, 5, 30, 'buff', 75, 20, 0, 0),
('Молитва Раков', ' prayer of Cancer', 'Молитва для Раков, призывающая защиту и семью', ' Prayer of Cancer, calling for protection and family', 4, 1, 5, 30, 'buff', 25, 25, 0, 0),
('Молитва Льва', ' prayer of Leo', 'Молитва для Львов, призывающая славу и величие', ' Prayer of Leo, calling for fame and greatness', 5, 1, 5, 30, 'buff', 100, 25, 0, 0);
"
-- 2. PRAYER_DEFINITIONS — Определения молитв
(2, 'Телец', 'Taurus', 'earth', 'Venus', 'earth', 'Дева, Козерог', 2, 'green', 'Земной signs, стабильность и изобилие', 'Earth signs, stability and abundance'),
(3, 'Близнецы', 'Gemini', 'air', ' Меркурий', 'air', 'Весы, Водолей', 3, 'yellow', 'Воздушные signs, общаги и ум', 'Air signs, communication and intellect'),
(4, 'Рак', 'Cancer', 'water', 'Луна', 'water', 'Рак, Скорпион', 4, 'silver', 'Водные signs, эмоции и интуиция', 'Water signs, emotions and intuition'),
(5, 'Лев', 'Leo', 'fire', 'Солнце', 'fire', 'Овен, Стрелец', 5, 'gold', 'Огонь signs, лидерство и слава', 'Fire signs, leadership and fame'),
(6, 'Дева', 'Virgo', 'earth', ' Меркурий', 'earth', 'Телец, Козерог', 6, 'brown', 'Земные signs, анализ и точность', 'Earth signs, analysis and precision'),
(7, 'Весы', 'Libra', 'air', 'Венера', 'air', 'Близнецы, Стрелец', 7, 'blue', 'Воздушные signs, баланс и гармония', 'Air signs, balance and harmony'),
(8, 'Скорпион', 'Scorpio', 'water', 'Плутон', 'water', 'Рак, Дест', 8, 'maroon', 'Водные signs, интенсивность и трансформация', 'Water signs, intensity and transformation'),
(9, 'Стрелец', 'Sagittarius', 'fire', 'Юпитер', 'fire', 'Овен, Стрелец', 9, 'purple', 'Огонь signs, приключения и философия', 'Fire signs, adventures and philosophy'),
(10, 'Козерог', 'Capricorn', 'earth', 'Сатурн', 'earth', 'Телец, Козерог', 10, 'black', 'Земные signs, амбиции и дисциплина', 'Earth signs, ambition and discipline'),
(11, 'Водолей', 'Aquarius', 'air', 'Уран', 'air', 'Близнецы, Дест', 11, 'cyan', 'Воздушные signs, инновации и ориентировка', 'Air signs, innovation and orientation'),
(12, 'Рыбы', 'Pisces', 'water', 'Нептун', 'water', 'Рак, Скорпион', 12, 'white', 'Водные signs, интуиция и мудрость', 'Water signs, intuition and wisdom');
"
    id TINYINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(32) NOT NULL,
    name_en VARCHAR(32) NOT NULL,
    element VARCHAR(16) NOT NULL,
    ruling_planet VARCHAR(32) NOT NULL,
    elemental_power VARCHAR(32) NOT NULL,
    compatibility VARCHAR(64) NOT NULL,
    lucky_number INT UNSIGNED NOT NULL DEFAULT 0,
    lucky_color VARCHAR(16) NOT NULL DEFAULT 'white',
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL
) ENGINE=InnoDB;
"
-- Generated for MMORPG Black Materia

-- 1. ZODIAC_SIGNS — Определения зодиакальных знаков
-- Zodiac System SQL Tables
-- Generated for MMORPG Black Materia

-- 1. ZODIAC_SIGNS — Определения зодиакальных знаков
-- Zodiac System SQL Tables
-- Generated for MMORPG Black Materia

-- 1. ZODIAC_SIGNS — Определения зодиакальных знаков



