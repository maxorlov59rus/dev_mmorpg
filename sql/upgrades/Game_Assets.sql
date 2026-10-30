-- ============================================================================
-- Game_Assets.sql — Музыка, модели, лор, описания, иконки
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. MUSIC_TRACK_DEF — Музыкальные треки
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS music_track_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    file_path VARCHAR(256) NOT NULL,
    category VARCHAR(32) NOT NULL,
    duration_sec INT UNSIGNED NOT NULL DEFAULT 120,
    loop_track TINYINT UNSIGNED NOT NULL DEFAULT 1,
    volume FLOAT NOT NULL DEFAULT 0.7,
    description_ru TEXT,
    description_en TEXT,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO music_track_def (id, name_ru, name_en, file_path, category, duration_sec, loop_track, volume, description_ru, description_en, is_active) VALUES
(1, 'Тема главного меню', 'Main Menu Theme', 'Audio/Music/main_menu_theme', 'ui', 180, 1, 0.8, 'Эпическая тема главного меню.', 'Epic main menu theme.', 1),
(2, 'Амбиент города', 'City Ambient', 'Audio/Music/city_ambient', 'ambient', 240, 1, 0.6, 'Спокойная городская музыка.', 'Calm city music.', 1),
(3, 'Боевая тема', 'Combat Theme', 'Audio/Music/combat_theme', 'combat', 90, 1, 0.9, 'Динамичная боевая музыка.', 'Dynamic combat music.', 1),
(4, 'Босс: Цитадель Тьмы', 'Boss: Citadel', 'Audio/Music/boss_citadel', 'boss', 120, 1, 1.0, 'Эпическая тема босса.', 'Epic boss theme.', 1),
(5, 'Босс: Логово Дракона', 'Boss: Dragon Lair', 'Audio/Music/boss_dragon', 'boss', 150, 1, 1.0, 'Мощная тема дракона.', 'Powerful dragon theme.', 1),
(6, 'Данж: Подземелье Забытых', 'Dungeon: Forgotten', 'Audio/Music/dungeon_forgotten', 'dungeon', 200, 1, 0.7, 'Таинственная музыка данжа.', 'Mysterious dungeon music.', 1),
(7, 'Данж: Пещеры Теней', 'Dungeon: Shadows', 'Audio/Music/dungeon_shadows', 'dungeon', 180, 1, 0.7, 'Тёмная музыка пещер.', 'Dark cave music.', 1),
(8, 'Рейд: Цитадель Тьмы', 'Raid: Citadel', 'Audio/Music/raid_citadel', 'raid', 300, 1, 0.8, 'Эпическая рейдовая тема.', 'Epic raid theme.', 1),
(9, 'Рейд: Логово Дракона', 'Raid: Dragon Lair', 'Audio/Music/raid_dragon', 'raid', 280, 1, 0.8, 'Грандиозная тема рейда.', 'Grand raid theme.', 1),
(10, 'Инстанс: Разлом', 'Instance: Rift', 'Audio/Music/instance_rift', 'instance', 150, 1, 0.7, 'Мистическая музыка разлома.', 'Mystical rift music.', 1),
(11, 'Инстанс: Астральный План', 'Instance: Astral', 'Audio/Music/instance_astral', 'instance', 200, 1, 0.7, 'Космическая музыка.', 'Cosmic music.', 1),
(12, 'PvP: Битва Духов', 'PvP: Spirit Battle', 'Audio/Music/pvp_spirit_battle', 'event', 120, 1, 0.9, 'Динамичная PvP музыка.', 'Dynamic PvP music.', 1),
(13, 'Событие: Туманная Аномалия', 'Event: Misty', 'Audio/Music/event_misty', 'event', 180, 1, 0.8, 'Атмосферная музыка.', 'Atmospheric music.', 1),
(14, 'Событие: Сезонное', 'Event: Seasonal', 'Audio/Music/event_seasonal', 'event', 150, 1, 0.7, 'Праздничная музыка.', 'Festive music.', 1),
(15, 'Победа', 'Victory', 'Audio/Music/victory_fanfare', 'ui', 15, 0, 1.0, 'Фанфары победы.', 'Victory fanfare.', 1),

-- ---------------------------------------------------------------------------
-- 2. MODEL_DEF — 3D модели
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS model_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    model_path VARCHAR(256) NOT NULL,
    model_type VARCHAR(32) NOT NULL,
    poly_count INT UNSIGNED NOT NULL DEFAULT 1000,
    texture_size INT UNSIGNED NOT NULL DEFAULT 512,
    description_ru TEXT,
    description_en TEXT,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO model_def (id, name_ru, name_en, model_path, model_type, poly_count, texture_size, description_ru, description_en, is_active) VALUES
(1, 'Воин', 'Warrior', 'Models/Characters/warrior', 'character', 5000, 1024, 'Воин с доспехами и мечом.', 'Warrior with armor and sword.', 1),
(2, 'Жрец', 'Priest', 'Models/Characters/priest', 'character', 4500, 1024, 'Жрец с посохом.', 'Priest with staff.', 1),
(3, 'Маг', 'Mage', 'Models/Characters/mage', 'character', 4800, 1024, 'Маг с жезлом.', 'Mage with wand.', 1),
(4, 'Разбойник', 'Rogue', 'Models/Characters/rogue', 'character', 4200, 1024, 'Разбойник с кинжалами.', 'Rogue with daggers.', 1),
(5, 'Охотник', 'Hunter', 'Models/Characters/hunter', 'character', 4600, 1024, 'Охотник с луком.', 'Hunter with bow.', 1),
(6, 'Чернокнижник', 'Warlock', 'Models/Characters/warlock', 'character', 5200, 1024, 'Чернокнижник с гримуаром.', 'Warlock with grimoire.', 1),
(7, 'Слизень', 'Slime', 'Models/Monsters/slime', 'monster', 800, 256, 'Базовый монстр.', 'Basic monster.', 1),
(8, 'Волк', 'Wolf', 'Models/Monsters/wolf', 'monster', 1200, 512, 'Волк-хищник.', 'Predator wolf.', 1),
(9, 'Гарпия', 'Harpy', 'Models/Monsters/harpy', 'monster', 2000, 512, 'Гарпия.', 'Harpy.', 1),
(10, 'Паук', 'Spider', 'Models/Monsters/spider', 'monster', 1500, 512, 'Гигантский паук.', 'Giant spider.', 1),
(11, 'Босс: Страж Разлома', 'Boss: Rift Guardian', 'Models/Bosses/rift_guardian', 'boss', 8000, 2048, 'Босс-страж.', 'Rift guardian boss.', 1),
(12, 'Босс: Повелитель Искажённых', 'Boss: Distortion Lord', 'Models/Bosses/distortion_lord', 'boss', 10000, 2048, 'Босс-повелитель.', 'Distortion lord boss.', 1),
(13, 'Дух: Страж', 'Spirit: Guardian', 'Models/Spirits/guardian', 'npc', 2000, 512, 'Дух-страж.', 'Guardian spirit.', 1),
(14, 'Дух: Целитель', 'Spirit: Healer', 'Models/Spirits/healer', 'npc', 1800, 512, 'Дух-целитель.', 'Healer spirit.', 1),
(15, 'Дух: Разрушитель', 'Spirit: Destroyer', 'Models/Spirits/destroyer', 'npc', 2200, 512, 'Дух-разрушитель.', 'Destroyer spirit.', 1),
(16, 'Дух: Поддержка', 'Spirit: Support', 'Models/Spirits/support', 'npc', 1900, 512, 'Дух-поддержка.', 'Support spirit.', 1);

-- ---------------------------------------------------------------------------
-- 3. ICON_DEF — Иконки
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS icon_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(64) NOT NULL,
    file_path VARCHAR(256) NOT NULL,
    category VARCHAR(32) NOT NULL,
    size_px INT UNSIGNED NOT NULL DEFAULT 64,
    description_ru VARCHAR(255),
    description_en VARCHAR(255),
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO icon_def (id, name, file_path, category, size_px, description_ru, description_en, is_active) VALUES
(1, 'icon_warrior', 'Icons/Classes/warrior', 'class', 64, 'Воин', 'Warrior', 1),
(2, 'icon_priest', 'Icons/Classes/priest', 'class', 64, 'Жрец', 'Priest', 1),
(3, 'icon_mage', 'Icons/Classes/mage', 'class', 64, 'Маг', 'Mage', 1),
(4, 'icon_rogue', 'Icons/Classes/rogue', 'class', 64, 'Разбойник', 'Rogue', 1),
(5, 'icon_hunter', 'Icons/Classes/hunter', 'class', 64, 'Охотник', 'Hunter', 1),
(6, 'icon_warlock', 'Icons/Classes/warlock', 'class', 64, 'Чернокнижник', 'Warlock', 1),
(7, 'icon_spirit_guardian', 'Icons/Spirits/guardian', 'spirit', 64, 'Страж', 'Guardian', 1),
(8, 'icon_spirit_healer', 'Icons/Spirits/healer', 'spirit', 64, 'Целитель', 'Healer', 1),
(9, 'icon_spirit_destroyer', 'Icons/Spirits/destroyer', 'spirit', 64, 'Разрушитель', 'Destroyer', 1),
(10, 'icon_spirit_support', 'Icons/Spirits/support', 'spirit', 64, 'Поддержка', 'Support', 1),
(11, 'icon_quality_common', 'Icons/Quality/common', 'ui', 32, 'Обычный', 'Common', 1),
(12, 'icon_quality_rare', 'Icons/Quality/rare', 'ui', 32, 'Редкий', 'Rare', 1),
(13, 'icon_quality_epic', 'Icons/Quality/epic', 'ui', 32, 'Эпический', 'Epic', 1),
(14, 'icon_quality_legendary', 'Icons/Quality/legendary', 'ui', 32, 'Легендарный', 'Legendary', 1),
(15, 'icon_mining', 'Icons/Professions/mining', 'profession', 64, 'Горное дело', 'Mining', 1),
(16, 'icon_herbalism', 'Icons/Professions/herbalism', 'profession', 64, 'Травничество', 'Herbalism', 1),
(17, 'icon_skinning', 'Icons/Professions/skinning', 'profession', 64, 'Снятие шкур', 'Skinning', 1),
(18, 'icon_blacksmithing', 'Icons/Professions/blacksmithing', 'profession', 64, 'Кузнечное дело', 'Blacksmithing', 1),
(19, 'icon_alchemy', 'Icons/Professions/alchemy', 'profession', 64, 'Алхимия', 'Alchemy', 1),
(20, 'icon_tailoring', 'Icons/Professions/tailoring', 'profession', 64, 'Портняжное дело', 'Tailoring', 1),
(21, 'icon_engineering', 'Icons/Professions/engineering', 'profession', 64, 'Инженерия', 'Engineering', 1),
(22, 'icon_jewelcrafting', 'Icons/Professions/jewelcrafting', 'profession', 64, 'Ювелирное дело', 'Jewelcrafting', 1);

-- ---------------------------------------------------------------------------
-- 4. LORE_DEF — Лор и описания
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lore_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    category VARCHAR(32) NOT NULL,
    entity_id INT UNSIGNED NOT NULL DEFAULT 0,
    title_ru VARCHAR(128) NOT NULL,
    title_en VARCHAR(128) NOT NULL,
    text_ru TEXT NOT NULL,
    text_en TEXT NOT NULL,
    author_ru VARCHAR(64),
    author_en VARCHAR(64),
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO lore_def (id, category, entity_id, title_ru, title_en, text_ru, text_en, author_ru, author_en, is_active) VALUES
(1, 'world', 0, 'Сотворение мира', 'Creation of the World', 'В начале был Хаос. Из Хаоса родились три силы: Свет, Тьма и Пустота.', 'In the beginning there was Chaos. From Chaos three forces were born: Light, Darkness, and Void.', 'Неизвестный', 'Unknown', 1),
(2, 'faction', 1, 'Ковенант Рассвета', 'Covenant of Dawn', 'Фракция света и порядка. Основана первыми людьми.', 'Faction of light and order. Founded by first humans.', 'Летописец Веларион', 'Chronicler Velarion', 1),
(3, 'faction', 2, 'Круг Полых', 'Circle of the Hollow', 'Фракция тьмы и некромантии.', 'Faction of darkness and necromancy.', 'Летописец Морвен', 'Chronicler Morven', 1),
(4, 'faction', 3, 'Гильдии Предела', 'Threshold Guilds', 'Фракция торговцев и наёмников.', 'Faction of merchants and mercenaries.', 'Летописец Гримм', 'Chronicler Grimm', 1),
(5, 'class', 1, 'Воин', 'Warrior', 'Мастера ближнего боя. Сила и выносливость.', 'Masters of melee. Strength and stamina.', 'Тренер Веларион', 'Trainer Velarion', 1),
(6, 'class', 2, 'Жрец', 'Priest', 'Целители и защитники. Магия света.', 'Healers and protectors. Light magic.', 'Тренер Морвен', 'Trainer Morven', 1),
(7, 'class', 3, 'Маг', 'Mage', 'Повелители стихий. Урон на расстоянии.', 'Masters of elements. Ranged damage.', 'Тренер Азариэль', 'Trainer Azariel', 1),
(8, 'class', 4, 'Разбойник', 'Rogue', 'Мастера скрытности. Критические удары.', 'Masters of stealth. Critical strikes.', 'Тренер Теневой Клинок', 'Trainer Shadowblade', 1),
(9, 'class', 5, 'Охотник', 'Hunter', 'Мастера дальнего боя. Приручение зверей.', 'Masters of ranged combat. Taming beasts.', 'Тренер Следопыт', 'Trainer Pathfinder', 1),
(10, 'class', 6, 'Чернокнижник', 'Warlock', 'Повелители тьмы и демонов.', 'Masters of darkness and demons.', 'Тренер Пустоты', 'Trainer Voidwalker', 1),
(11, 'race', 1, 'Человек', 'Human', 'Самая многочисленная раса.', 'Most numerous race.', 'Летописец', 'Chronicler', 1),
(12, 'race', 2, 'Люминар', 'Lumenar', 'Светящиеся существа с магической силой.', 'Glowing beings with magical power.', 'Летописец', 'Chronicler', 1),
(13, 'race', 3, 'Пепельный', 'Ashen', 'Выжившие после катастрофы.', 'Survivors of catastrophe.', 'Летописец', 'Chronicler', 1),
(14, 'race', 4, 'Полый', 'Hollowed', 'Существа, потерявшие часть души.', 'Beings who lost part of their soul.', 'Летописец', 'Chronicler', 1),
(15, 'race', 5, 'Пределкин', 'Thresholdkin', 'Потомки торговцев с хитростью.', 'Descendants of merchants with cunning.', 'Летописец', 'Chronicler', 1);

-- ---------------------------------------------------------------------------
-- 5. ASSET_CONFIG — Конфигурация ассетов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS asset_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO asset_config (config_key, config_value, comment) VALUES
('assets_enabled', 'true', 'Включить ассеты'),
('music_enabled', 'true', 'Включить музыку'),
('models_enabled', 'true', 'Включить 3D модели'),
('icons_enabled', 'true', 'Включить иконки'),
('lore_enabled', 'true', 'Включить лор'),
('default_music_volume', '0.7', 'Громкость музыки'),
('default_sfx_volume', '0.8', 'Громкость звуков');
(16, 'Поражение', 'Defeat', 'Audio/Music/defeat', 'ui', 10, 0, 1.0, 'Музыка поражения.', 'Defeat music.', 1);