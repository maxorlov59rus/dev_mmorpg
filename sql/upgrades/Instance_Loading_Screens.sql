-- ============================================================================
-- Instance_Loading_Screens.sql — Динамические анимированные логинскрины
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ---------------------------------------------------------------------------
-- 1. INSTANCE_LOADING_SCREEN_DEF — Определение логинскринов
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS instance_loading_screen_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    instance_type TINYINT UNSIGNED NOT NULL COMMENT '1=dungeon, 2=raid, 3=instance',
    map_id INT UNSIGNED NOT NULL,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    background_image VARCHAR(128) NOT NULL,
    animation_type TINYINT UNSIGNED NOT NULL DEFAULT 1 COMMENT '1=fade, 2=slide, 3=zoom, 4=particle',
    animation_duration_ms INT UNSIGNED NOT NULL DEFAULT 3000,
    tip_ru TEXT,
    tip_en TEXT,
    music_track VARCHAR(128),
    min_level TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

INSERT IGNORE INTO instance_loading_screen_def (id, instance_type, map_id, name_ru, name_en, background_image, animation_type, animation_duration_ms, tip_ru, tip_en, music_track, min_level, is_active) VALUES
(1, 1, 2001, 'Подземелье Забытых', 'Dungeon of the Forgotten', 'bg_dungeon_forgotten', 1, 3000, 'Используйте факелы для освещения тёмных коридоров.', 'Use torches to light dark corridors.', 'music_dungeon_ambient', 10, 1),
(2, 1, 2002, 'Пещеры Теней', 'Caves of Shadows', 'bg_dungeon_shadows', 2, 3500, 'Остерегайтесь ловушек на полу.', 'Beware of floor traps.', 'music_dungeon_dark', 15, 1),
(3, 1, 2003, 'Руины Древних', 'Ruins of the Ancients', 'bg_dungeon_ruins', 3, 4000, 'Ищите скрытые проходы за стенами.', 'Look for hidden passages behind walls.', 'music_dungeon_epic', 20, 1),
(4, 2, 3001, 'Рейд: Цитадель Тьмы', 'Raid: Citadel of Darkness', 'bg_raid_citadel', 4, 5000, 'Координация группы критически важна.', 'Group coordination is critical.', 'music_raid_epic', 30, 1),
(5, 2, 3002, 'Рейд: Логово Дракона', 'Raid: Dragon Lair', 'bg_raid_dragon', 1, 4500, 'Избегайте огненного дыхания дракона.', 'Avoid the dragon breath.', 'music_raid_dragon', 40, 1),
(6, 3, 4001, 'Инстанс: Разлом', 'Instance: Rift', 'bg_instance_rift', 2, 3000, 'Разлом искажает пространство и время.', 'The rift distorts space and time.', 'music_instance_mystic', 25, 1),
(7, 3, 4002, 'Инстанс: Астральный План', 'Instance: Astral Plane', 'bg_instance_astral', 3, 3500, 'Астральные существа невидимы для обычных атак.', 'Astral beings are invisible to normal attacks.', 'music_instance_cosmic', 35, 1);

-- ---------------------------------------------------------------------------
-- 2. INSTANCE_LOADING_SCREEN_CONFIG — Конфигурация
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS instance_loading_screen_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO instance_loading_screen_config (config_key, config_value, comment) VALUES
('loading_screens_enabled', 'true', 'Включить логинскрины'),
('default_animation_type', '1', 'Тип анимации по умолчанию (1=fade)'),
('default_duration_ms', '3000', 'Длительность по умолчанию (мс)'),
('show_tips', 'true', 'Показывать советы'),
('show_progress_bar', 'true', 'Показывать прогресс-бар'),
('fade_in_ms', '500', 'Время fade-in (мс)'),
('fade_out_ms', '500', 'Время fade-out (мс)'),
('min_display_time_ms', '2000', 'Минимальное время показа (мс)');

-- ---------------------------------------------------------------------------
-- 3. INSTANCE_LOADING_SCREEN_LOG — Лог загрузок (опционально)
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS instance_loading_screen_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    player_guid BIGINT UNSIGNED NOT NULL,
    map_id INT UNSIGNED NOT NULL,
    loading_time_ms INT UNSIGNED NOT NULL,
    entered_at INT UNSIGNED NOT NULL,
    INDEX idx_player (player_guid),
    INDEX idx_map (map_id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 4. NPC — Хранитель Порталов (для входа в инстансы)
-- ---------------------------------------------------------------------------
INSERT IGNORE INTO creature_template (entry, name, subname, level, type, faction_alliance, faction_horde, min_level, max_level, dmg_modifier, health_modifier, lootid, pickpocketloot, mingold, maxgold, unit_flags, mechanic_immune_mask) VALUES
(95001, 'Хранитель Порталов', 'Страж Врат', 20, 7, 1650, 1651, 20, 20, 1.0, 1.0, 0, 0, 0, 0, 0, 0);

INSERT IGNORE INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
(95001, 1, 65536*100, 65536*80, 65536*10, 0x5);