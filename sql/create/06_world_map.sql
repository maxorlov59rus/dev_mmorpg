-- =============================================================================
-- «Чёрная Материя» — sql/create/06_world_map.sql
-- Карта локаций (аналог TRC map/area/instance tables) + правила зон PvP/PvE.
-- База: dm_world. Источник истины для ZoneManager (dm-server/src/game/Grids).
--
-- Контракты:
--   * areatable.category  -> EZoneCategory клиента (UI_MapWidget):
--                            1 City(Sanctuary) 2 PvE-Zone 3 FFA 4 Arena
--                            5 Dungeon 6 Raid 7 Wilderness(PvP-flagged possible)
--   * pvp_zones.ruleset   -> ZoneRuleSet сервера / defs v1.7:
--                            1 Sanctuary / 2 Arena / 3 FFA
--     (dm_world.pvp_zones — канон из PvE_PvP_Core.sql; здесь только дополнения
--      ON DUPLICATE KEY UPDATE для новых зон карты, идемпотентно).
--   * SMSG_ZONE_MAP 0x204A отдаёт клиенту строки этого файла (join areatable +
--     pvp_zones + graveyard_binding), см. proto/opcodes.defs v1.8.
-- =============================================================================
USE dm_world;

-- -----------------------------------------------------------------------------
-- 1. map — континенты/planes (аналог TRC `map`)
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `map` (
    `id`          SMALLINT UNSIGNED NOT NULL,
    `name`        VARCHAR(128) NOT NULL,
    `category`    TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 world 1 instanced',
    `min_player`  TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `max_player`  SMALLINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 = open world',
    `ue_level`    VARCHAR(191) NOT NULL DEFAULT '' COMMENT '/Game/Maps/L_<Name> UE asset path',
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `map` (`id`, `name`, `category`, `min_player`, `max_player`, `ue_level`) VALUES
(0,   'Картос — Центральные земли',       0, 0, 0, '/Game/Maps/L_World_Cartos'),
(1,   'Рассветный Предел (фронтир Альянса)', 0, 0, 0, '/Game/Maps/L_Front_Dawn'),
(2,   'Ржавые Пустоши (FFA)',             0, 0, 0, '/Game/Maps/L_RustWastes'),
(3,   'Пепельные Марши (фронтир Ордена)', 0, 0, 0, '/Game/Maps/L_Front_Ash'),
(4,   'Пороговая Пустота (ренегаты)',     0, 0, 0, '/Game/Maps/L_NoMansLand'),
(400, 'Арена: Кольцо Первого Света',      1, 6, 6,  '/Game/Maps/Inst/L_Arena_RingFirstLight'),
(401, 'Арена:Void Pit (дуэльный полигон)',1, 2, 2,  '/Game/Maps/Inst/L_Arena_VoidPit'),
(402, 'Арена: Пороговые Ворота (3v3)',    1, 6, 6,  '/Game/Maps/Inst/L_Arena_ThresholdGate'),
(500, 'Инстанс: Пещеры Испытаний',        1, 5, 5,  '/Game/Maps/Inst/L_Dun_TrialCaves'),
(501, 'Инстанс: Разлом Истечения (L40)',  1, 5, 5,  '/Game/Maps/Inst/L_Dun_SeepageRift'),
(502, 'Инстанс: Крепость Поглощения (L60)',1,5, 5,  '/Game/Maps/Inst/L_Dun_DevourKeep'),
(600, 'Рейд: Чёрная Звезда (L80, 20м)',   1, 20, 20,'/Game/Maps/Raid/L_Raid_BlackStar'),
(601, 'Рейд: Сердце Материи (L95, 20м)',  1, 20, 20,'/Game/Maps/Raid/L_Raid_CoreOfMatter')
ON DUPLICATE KEY UPDATE `name`=VALUES(`name`), `ue_level`=VALUES(`ue_level`);

-- -----------------------------------------------------------------------------
-- 2. areatable — каждая локация/зона карты (аналог TRC `areatable`)
--    zone_id = AreaID в пакете SMSG_PVP_ZONE_INFO / SMSG_ZONE_MAP.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `areatable` (
    `id`           INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id`       SMALLINT UNSIGNED NOT NULL,
    `zone_id`      SMALLINT UNSIGNED NOT NULL COMMENT '= area_id в pvp_zones/SMSG',
    `name`         VARCHAR(128) NOT NULL,
    `category`     TINYINT UNSIGNED NOT NULL COMMENT 'EZoneCategory (см. шапку)',
    `min_level`    TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `max_level`    TINYINT UNSIGNED NOT NULL DEFAULT 95,
    `faction_id`   TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 neutral / 1 Орден Рассвета / 2 Круг Поглощения',
    `parent_area`  INT UNSIGNED NOT NULL DEFAULT 0,
    `flag_pve`     TINYINT UNSIGNED NOT NULL DEFAULT 1 COMMENT '1 = мобов/квесты можно проходить',
    `flag_duelable` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `mount_allowed` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    PRIMARY KEY (`id`),
    UNIQUE KEY `uk_map_zone` (`map_id`, `zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --- Мир 0: центральные земли -------------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(0, 1,  'Столица: Заря Рассвета (Dawnhold)',        1, 1, 95, 1, 1, 0, 0), -- Sanctuary city Alliance
(0, 2,  'Столица: Пепельный Предел (Ashen Reach)',  1, 1, 95, 2, 1, 0, 0), -- Sanctuary city Ember
(0, 3,  'Торговый узел: Пороговые Врата',            1, 1, 95, 0, 1, 0, 0), -- Neutral hub Sanctuary
(0, 4,  'Луга Пробуждения (стартовая, Альянс)',      2, 1, 5,  1, 1, 1, 1),
(0, 5,  'Опалинные Холмы (стартовая, Орден)',        2, 1, 5,  2, 1, 1, 1),
(0, 6,  'Тихая Долина (нейтральный город-лагерь)',   1, 5, 15, 0, 1, 0, 1),
(0, 7,  'Серебристый Лес',                           2, 6, 15, 0, 1, 1, 1),
(0, 8,  'Мшистые Топи',                              2, 10, 20, 0, 1, 1, 1),
(0, 9,  'Каменные Карьеры Этерниума',                7, 20, 30, 0, 1, 1, 1), -- Wilderness PvP-possible
(0, 10, 'Предгорья Стужи',                           2, 25, 35, 0, 1, 1, 1),
(0, 11, 'Руины Валдарена',                           7, 35, 45, 0, 1, 1, 1),
(0, 12, 'Пепельная Пуща',                            2, 40, 50, 2, 1, 1, 1),
(0, 13, 'Степи Рассветного Флага',                   2, 45, 55, 1, 1, 1, 1),
(0, 14, 'Затопленные Катакомбы (поверх.)',           7, 50, 60, 0, 1, 1, 0),
(0, 15, 'Нагорье Чернокнижников',                    7, 55, 65, 0, 1, 1, 1),
(0, 16, 'Ледник Молчания',                           2, 60, 70, 0, 1, 1, 1),
(0, 17, 'Граница Истечения (Seepage Front)',         7, 70, 80, 0, 1, 1, 1),
(0, 18, 'Выжженное Побережье',                       7, 75, 85, 0, 1, 1, 1),
(0, 19, 'Цитадель Порога (эндзон ренегатов)',        3, 85, 95, 0, 0, 1, 1), -- FFA endgame hub
(0, 20, 'Вечный Дозор (санктуарий эндгейма)',        1, 85, 95, 0, 1, 0, 0);
-- --- Мир 1/3: фронтиры фракций ------------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(1, 101, 'Дозор Восхода (город-крепость)',            1, 10, 95, 1, 1, 0, 0),
(1, 111, 'Пределы Рассвета — поля',                   2, 10, 30, 1, 1, 1, 1),
(1, 112, 'Башни Первой Зари (линия фронта)',          7, 30, 45, 1, 1, 1, 1),
(1, 113, 'Истечение Аль-Таир (PvE-роёвня)',           2, 40, 55, 1, 1, 1, 1),
(3, 301, 'Оплот Пепла (город-крепость)',              1, 10, 95, 2, 1, 0, 0),
(3, 311, 'Жернова Пепельных Маршей',                  2, 10, 30, 2, 1, 1, 1),
(3, 312, 'Костяные Ряды (линия фронта)',              7, 30, 45, 2, 1, 1, 1),
(3, 313, 'Кузница Поглощения (PvE-роёвня)',           2, 40, 55, 2, 1, 1, 1);
-- --- Мир 2: Ржавые Пустоши (FFA) ----------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(2, 201, 'Ржавые Пустоши — Работорговый Рынок',       3, 10, 25, 0, 1, 1, 1),
(2, 202, 'Свалка Левиафанов',                         3, 15, 30, 0, 1, 1, 1),
(2, 203, 'Красные Каньоны',                           3, 25, 40, 0, 1, 1, 1),
(2, 204, 'Лагерь Изгоев (ренегаты, FFA-safe corner)', 3, 10, 95, 0, 1, 1, 1),
(2, 205, 'Яма Гладиадоров (PvP-аванпост)',            3, 30, 50, 0, 0, 1, 1);
-- --- Мир 4: Пороговая Пустота (ренегаты/high-end) ------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(4, 411, 'Разломанные Столпы',                        3, 50, 65, 0, 1, 1, 1),
(4, 412, 'Поле Последней Ссоры',                      3, 60, 75, 0, 0, 1, 1),
(4, 413, 'Чёрный Рынок (нейтральная торговля)',       1, 50, 95, 0, 1, 0, 0); -- Sanctuary enclave
-- --- Арены (400..402) ------------------------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(400, 401, 'Кольцо Первого Света (3v3)',              4, 10, 95, 0, 0, 1, 0),
(401, 402, 'Void Pit (дуэльный полигон 1v1)',         4, 10, 95, 0, 0, 1, 0),
(402, 403, 'Пороговые Ворота (3v3 рейтинговая)',      4, 20, 95, 0, 0, 1, 0);
-- --- Инстансы (500..502) ----------------------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(500, 501, 'Пещеры Испытаний (обучение LFG)',         5, 10, 15, 0, 1, 0, 0),
(501, 511, 'Разлом Истечения',                        5, 38, 45, 0, 1, 0, 0),
(501, 512, 'Разлом Истечения — Легендарный режим',    5, 40, 46, 0, 1, 0, 0),
(502, 521, 'Крепость Поглощения',                     5, 58, 65, 0, 1, 0, 0),
(502, 522, 'Крепость Поглощения — Мифик',             5, 60, 66, 0, 1, 0, 0);
-- --- Рейды (600..601) --------------------------------------------------------------
INSERT INTO `areatable` (`map_id`,`zone_id`,`name`,`category`,`min_level`,`max_level`,`faction_id`,`flag_pve`,`flag_duelable`,`mount_allowed`) VALUES
(600, 601, 'Чёрная Звезда — пролог',                  6, 78, 82, 0, 1, 0, 0),
(600, 602, 'Чёрная Звезда — ядро рейда',              6, 80, 85, 0, 1, 0, 0),
(601, 611, 'Сердце Материи — внешние залы',           6, 92, 95, 0, 1, 0, 0),
(601, 612, 'Сердце Материи — Престол Истечения',      6, 95, 95, 0, 1, 0, 0);

-- -----------------------------------------------------------------------------
-- 3. Дополнение к pvp_zones (канон в PvE_PvP_Core.sql). Идемпотентно через
--    uk_map_area. Правила для всех новых зон карты.
-- -----------------------------------------------------------------------------
INSERT INTO `pvp_zones`
    (`map_id`,`area_id`,`name`,`ruleset`,`forced_untag`,`auto_resurrect`,`clean_debuffs`,`honor_mult_q16`)
VALUES
-- Sanctuary (ruleset=1): города, хабы, стартовые мирные зоны
(0, 1,  'Dawnhold Sanctuary',                 1, 1, 0, 0, 65536),
(0, 2,  'AshenReach Sanctuary',              1, 1, 0, 0, 65536),
(0, 3,  'ThresholdGate Sanctuary',           1, 1, 0, 0, 65536),
(0, 4,  'Meadows of Awakening (peace)',      1, 1, 0, 0, 65536),
(0, 5,  'Scorched Hills (peace)',            1, 1, 0, 0, 65536),
(0, 6,  'Quiet Vale (peace)',                1, 1, 0, 0, 65536),
(0, 20, 'Everwatch Sanctuary',               1, 1, 0, 0, 65536),
(1, 101,'Dawn Patrol Fortress (safe)',       1, 1, 0, 0, 65536),
(3, 301,'Ash Bastion (safe)',                1, 1, 0, 0, 65536),
(4, 413,'Black Market (neutral safe)',       1, 1, 0, 0, 65536),
-- Wilderness (ruleset=3 = FFA-raid style open PvP): пустоши + фронты + эндзон
(2, 201,'Rust Wastes Market (FFA)',          3, 0, 0, 0, 98304),
(2, 202,'Leviathan Junkyard (FFA)',          3, 0, 0, 0, 98304),
(2, 203,'Red Canyons (FFA)',                 3, 0, 0, 0, 98304),
(2, 204,'Outcast Camp (FFA, parley tile)',   3, 0, 0, 0, 98304),
(2, 205,'Gladiator Pit (FFA)',               3, 0, 0, 0, 131072),
(4, 411,'Broken Pillars (FFA)',              3, 0, 0, 0, 98304),
(4, 412,'Field of Last Strife (FFA)',        3, 0, 0, 0, 131072),
(0, 19, 'Threshold Citadel (FFA endgame)',   3, 0, 0, 0, 131072),
(0, 9,  'Eternium Quarries (contested)',     3, 0, 0, 0, 65536),
(0, 11, 'Ruins of Valdaren (contested)',     3, 0, 0, 0, 65536),
(0, 14, 'Flooded Catacombs surface (contested)',3,0,0,0, 65536),
(0, 15, 'Warlocks Upland (contested)',       3, 0, 0, 0, 65536),
(0, 17, 'Seepage Front (contested)',         3, 0, 0, 0, 98304),
(0, 18, 'Scorched Coast (contested)',        3, 0, 0, 0, 98304),
(1, 112,'Towers of First Light (front)',     3, 0, 0, 0, 98304),
(3, 312,'Bone Ranks (front)',                3, 0, 0, 0, 98304),
-- Arenas (ruleset=2)
(400, 401,'Ring of First Light (3v3)',       2, 0, 1, 1, 131072),
(401, 402,'Void Pit (duel ground)',          2, 0, 1, 1, 131072),
(402, 403,'Threshold Gate Arena (3v3)',      2, 0, 1, 1, 131072)
ON DUPLICATE KEY UPDATE `ruleset`=VALUES(`ruleset`), `forced_untag`=VALUES(`forced_untag`),
    `auto_resurrect`=VALUES(`auto_resurrect`), `clean_debuffs`=VALUES(`clean_debuffs`),
    `honor_mult_q16`=VALUES(`honor_mult_q16`), `name`=VALUES(`name`);

-- -----------------------------------------------------------------------------
-- 4. graveyard_binding — кладбища для PvE-политики смерти (mode=pve_graveyard).
--    Читается PvEEncounterMgr::OnPlayerDied; колонки совпадают с PvE_PvP_Core.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `graveyard_binding` (
    `gy_id`        INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id`       SMALLINT UNSIGNED NOT NULL,
    `zone_area_id` SMALLINT UNSIGNED NOT NULL,
    `gy_map`       SMALLINT UNSIGNED NOT NULL,
    `pos_x`        FLOAT NOT NULL DEFAULT 0,
    `pos_y`        FLOAT NOT NULL DEFAULT 0,
    `pos_z`        FLOAT NOT NULL DEFAULT 0,
    PRIMARY KEY (`gy_id`),
    UNIQUE KEY `uk_bind` (`map_id`,`zone_area_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `graveyard_binding` (`map_id`,`zone_area_id`,`gy_map`,`pos_x`,`pos_y`,`pos_z`) VALUES
(0, 4,  0, -8200,  1500, 45),   -- Meadows -> Dawnhold cemetery
(0, 5,  0,  8200, -1500, 45),   -- Scorched Hills -> Ashen Reach cemetery
(0, 7,  0, -4100,  6200, 12),
(0, 8,  0,  3300, -5400, -8),
(0, 9,  0,  1200,  9800, 120),
(0, 10, 0, -6600,  7700, 310),
(0, 11, 0,  2050, -2250, 60),
(0, 12, 0, -1900, -8800, 30),
(0, 13, 0,  7400,  4100, 90),
(0, 16, 0, -9300, -3900, 480),
(0, 17, 0,  5500,  5500, 25),
(0, 18, 0,  8800, -7200, 5),
(1, 111,1, 1100, -2300, 70),
(1, 113,1, -3400, 5100, 15),
(3, 311,3, 2200, 3300, 40),
(3, 313,3, -5100,-1200, 10),
(2, 201,2, 0, 0, 10),
(2, 202,2, 1500, -900, 5),
(2, 203,2, -2100, 1800, 220),
(4, 411,4, 900, 900, 30),
(4, 412,4, -700, -700, 12)
ON DUPLICATE KEY UPDATE `gy_map`=VALUES(`gy_map`), `pos_x`=VALUES(`pos_x`), `pos_y`=VALUES(`pos_y`), `pos_z`=VALUES(`pos_z`);

-- -----------------------------------------------------------------------------
-- 5. areatrigger_teleport — точки входа в инстансы/города (аналог TRC).
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `areatrigger_teleport` (
    `trigger_id`   INT UNSIGNED NOT NULL,
    `from_map`     SMALLINT UNSIGNED NOT NULL,
    `from_area`    SMALLINT UNSIGNED NOT NULL,
    `to_map`       SMALLINT UNSIGNED NOT NULL,
    `to_area`      SMALLINT UNSIGNED NOT NULL,
    `to_x`         FLOAT NOT NULL DEFAULT 0,
    `to_y`         FLOAT NOT NULL DEFAULT 0,
    `to_z`         FLOAT NOT NULL DEFAULT 0,
    `min_level`    TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `quest_required` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'q_1105 gate для Пещер Испытаний',
    `instance_lock_check` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '1 = сверять instance_locks',
    PRIMARY KEY (`trigger_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `areatrigger_teleport`
    (`trigger_id`,`from_map`,`from_area`,`to_map`,`to_area`,`to_x`,`to_y`,`to_z`,`min_level`,`quest_required`,`instance_lock_check`) VALUES
(7001, 0, 6,  500, 501, 0, 0, 10, 10, 1105, 1),  -- Trial Caves entrance (LFG tutorial gate q_1105)
(7002, 0, 10, 501, 511, 0, 0, 20, 38, 0, 1),     -- Seepage Rift normal
(7003, 0, 10, 501, 512, 0, 0, 20, 40, 0, 1),     -- Seepage Rift legendary
(7004, 0, 17, 502, 521, 0, 0, 5,  58, 0, 1),     -- Devour Keep
(7005, 0, 17, 502, 522, 0, 0, 5,  60, 0, 1),     -- Devour Keep mythic
(7006, 0, 20, 600, 601, 0, 0, 40, 78, 0, 1),     -- Black Star raid prologue
(7007, 0, 20, 601, 611, 0, 0, 30, 92, 0, 1),     -- Heart of Matter
(7008, 2, 204, 400, 401, 0, 0, 12, 10, 0, 0),    -- Outcast Camp -> Ring of First Light queue
(7009, 0, 3,  402, 403, 0, 0, 12, 20, 0, 0),     -- Threshold Gate -> rated arena
(7010, 0, 1,  401, 402, 0, 0, 8,  10, 0, 0);     -- Dawnhold duel grounds

-- -----------------------------------------------------------------------------
-- 6. Представление для SMSG_ZONE_MAP (сервер делает SELECT из него).
-- -----------------------------------------------------------------------------
CREATE OR REPLACE VIEW `v_zone_map_export` AS
SELECT a.`map_id`, a.`zone_id` AS `area_id`, a.`name`, a.`category`,
       a.`min_level`, a.`max_level`, a.`faction_id`,
       IFNULL(p.`ruleset`, 0) AS `ruleset`,
       IFNULL(p.`honor_mult_q16`, 65536) AS `honor_mult_q16`,
       IFNULL(g.`gy_map`, 65535) AS `graveyard_map`
FROM `areatable` a
LEFT JOIN `pvp_zones` p ON p.`map_id` = a.`map_id` AND p.`area_id` = a.`zone_id`
LEFT JOIN `graveyard_binding` g ON g.`map_id` = a.`map_id` AND g.`zone_area_id` = a.`zone_id`;
