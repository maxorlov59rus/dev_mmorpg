-- =====================================================================
-- PvE/PvP ядро: схема + данные. База dm_world (аналог TrinityCore world DB).
-- Контракт флагов совпадает с proto/opcodes.defs v1.7 и PvPSystem.h:
--   1=PVP_FLAGGED 2=FFA 4=IN_SANCTUARY 8=DUELING 16=WANTED 32=DISHONORED
-- RuleSet зон: 1=Sanctuary 2=Arena 3=FFA
-- =====================================================================

USE dm_world;

-- ---------------------------------------------------------------------
-- 1. character_flags — серверный источник истины PvP-статуса и таймеров.
--    Пишется из PvPSystem::PersistFlags() при каждом изменении флага
--    (debounce 5 сек) и полностью при logout. Читается при login ->
--    SMSG_PVP_FLAG_UPDATE (0x2041) в стартовый пакет игрока.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `character_flags` (
    `guid`              INT UNSIGNED NOT NULL,
    `pvp_flags`         SMALLINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'bitmask EPvPFlag',
    `flag_expire_time`  INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'unix-время авто-снятия PVP_FLAGGED (combat timeout)',
    `honor_points`      INT UNSIGNED NOT NULL DEFAULT 0,
    `wanted_level`      TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'PvP Bounty 0..5',
    `dishonor_expire`   INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Dishonorable Death: запрет групп до unix-time (2ч)',
    `duel_id`           BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '>0 => активная дуэль (сбрасывается при reload)',
    PRIMARY KEY (`guid`),
    CONSTRAINT `fk_cf_char` FOREIGN KEY (`guid`) REFERENCES `characters` (`guid`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 2. pvp_kills_log — статистика/античит/рейтинги. Пишется из
--    PvPSystem::OnPlayerKilledPlayer() в той же транзакции, что и
--    потеря репутации (ReputationMgr::ModifyReputation).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `pvp_kills_log` (
    `id`            BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `killer_guid`   INT UNSIGNED NOT NULL,
    `victim_guid`   INT UNSIGNED NOT NULL,
    `killer_faction` TINYINT UNSIGNED NOT NULL COMMENT '0 Dawn 1 Circle 2 Threshold 3 Renegade',
    `victim_faction` TINYINT UNSIGNED NOT NULL,
    `map_id`        SMALLINT UNSIGNED NOT NULL,
    `area_id`       SMALLINT UNSIGNED NOT NULL,
    `zone_ruleset`  TINYINT UNSIGNED NOT NULL COMMENT 'снимок pvp_zones.RuleSet на момент убийства',
    `rep_delta`     INT NOT NULL DEFAULT 0 COMMENT 'потерянная репутация (<0) или 0 (ренегат/враг)',
    `honor_gain`    SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `dishonored`    TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `timestamp`     INT UNSIGNED NOT NULL,
    PRIMARY KEY (`id`),
    KEY `idx_killer_ts` (`killer_guid`, `timestamp`),
    KEY `idx_victim_ts` (`victim_guid`, `timestamp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 3. pvp_zones — правила зон. Кэш: PvPSystem::LoadZoneCache() при старте
--    mapserver'а; проверка при OnEnterZone / CanAttack.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `pvp_zones` (
    `zone_id`        INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id`         SMALLINT UNSIGNED NOT NULL,
    `area_id`        SMALLINT UNSIGNED NOT NULL COMMENT 'UE Volume AreaID / TRC area',
    `name`           VARCHAR(128) NOT NULL,
    `ruleset`        TINYINT UNSIGNED NOT NULL COMMENT '1 Sanctuary / 2 Arena / 3 FFA',
    `forced_untag`   TINYINT UNSIGNED NOT NULL DEFAULT 1 COMMENT 'Sanctuary: принудительное снятие флага',
    `auto_resurrect` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Arena: Resurrect() при входе',
    `clean_debuffs`  TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Arena: снять дебаффы при входе',
    `honor_mult_q16` INT UNSIGNED NOT NULL DEFAULT 65536 COMMENT 'множитель чести, Q16 (FFA x1.5 = 98304)',
    PRIMARY KEY (`zone_id`),
    UNIQUE KEY `uk_map_area` (`map_id`, `area_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `pvp_zones` (`map_id`, `area_id`, `name`, `ruleset`, `forced_untag`, `auto_resurrect`, `clean_debuffs`, `honor_mult_q16`) VALUES
(1, 101, 'Dawnhold - City of the Order of the Dawn', 1, 1, 0, 0, 65536),
(1, 102, 'Ashen Reach - Citadel of the Ember Order', 1, 1, 0, 0, 65536),
(1, 103, 'Threshold Gate - Neutral Trade Hub',       1, 1, 0, 0, 65536),
(400, 401, 'Ring of First Light (3v3)',              2, 0, 1, 1, 131072),
(401, 402, 'Void Pit (1v1 Duel Ground)',             2, 0, 1, 1, 131072),
(2, 201, 'Rust Wastes (FFA Open World)',             3, 0, 0, 0, 98304);

-- ---------------------------------------------------------------------
-- 4. graveyard_binding — PvE-политика смерти: привязка зоны к кладбищу.
--    Читается PvEEncounterMgr::OnPlayerDied (mode=pve_graveyard).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `graveyard_binding` (
    `gy_id`        INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id`       SMALLINT UNSIGNED NOT NULL,
    `zone_area_id` SMALLINT UNSIGNED NOT NULL,
    `gy_map`       SMALLINT UNSIGNED NOT NULL COMMENT 'карта, куда телепортируется труп/дух',
    `pos_x` FLOAT NOT NULL DEFAULT 0, `pos_y` FLOAT NOT NULL DEFAULT 0, `pos_z` FLOAT NOT NULL DEFAULT 0,
    PRIMARY KEY (`gy_id`),
    KEY `idx_zone` (`map_id`, `zone_area_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `graveyard_binding` (`map_id`, `zone_area_id`, `gy_map`, `pos_x`, `pos_y`, `pos_z`) VALUES
(1, 101, 1, -210.0, 45.0, 90.0),
(1, 102, 1, 830.0, -120.0, 75.0),
(2, 201, 2, 120.0, 300.0, 40.0);

-- ---------------------------------------------------------------------
-- 5. spell_template (минимальная) — ID спеллов штрафов смерти зафиксированы
--    здесь и в defs v1.7 (sickness=80100).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `spell_template` (
    `spell_id`   MEDIUMINT UNSIGNED NOT NULL,
    `name`       VARCHAR(128) NOT NULL,
    `aura_type`  TINYINT UNSIGNED NOT NULL COMMENT '0 none 1 mod_pct_stats 2 threat_set 3 group_ban',
    `value_q16`  INT NOT NULL DEFAULT 0 COMMENT '-32768 == -50% (Q16)',
    `duration_sec` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`spell_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `spell_template` VALUES
(80100, 'Resurrection Sickness (-50% stats, 10 min)', 1, -32768, 600),
(80101, 'Taunt (Threat=999999, 3 sec)',               2, 999999, 3),
(80102, 'Dishonorable Death (no groups, 2 h)',        3, 0, 7200);

-- ---------------------------------------------------------------------
-- 6. hate_modifier_template — множители Threat = Damage * Modifiers
--    (PvEEncounterMgr::AddThreat).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `hate_modifier_template` (
    `modifier_id`  INT UNSIGNED NOT NULL,
    `name`         VARCHAR(64) NOT NULL,
    `mult_q16`     INT UNSIGNED NOT NULL COMMENT 'Q16: 65536=x1.0',
    PRIMARY KEY (`modifier_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `hate_modifier_template` VALUES
(1, 'base',                    65536),
(2, 'tank_stance',             98304),
(3, 'taunt_buff',              131072),
(4, 'healing_overheal',        32768),
(5, 'damage_reduction_hatred', 196608);

-- ---------------------------------------------------------------------
-- 7. creature_proto_aggro — параметры аггро мобов для PvE-ядра.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS `creature_proto_aggro` (
    `entry`        MEDIUMINT UNSIGNED NOT NULL,
    `aggro_range`  FLOAT UNSIGNED NOT NULL DEFAULT 16.0,
    `leash_range`  FLOAT UNSIGNED NOT NULL DEFAULT 30.0,
    `front_arc_deg` FLOAT UNSIGNED NOT NULL DEFAULT 120.0 COMMENT 'сектор от взгляда моба',
    `assist_radius` FLOAT UNSIGNED NOT NULL DEFAULT 0.0 COMMENT 'цепная аггро союзников, 0=нет',
    PRIMARY KEY (`entry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `creature_proto_aggro` (`entry`, `aggro_range`, `leash_range`, `front_arc_deg`, `assist_radius`) VALUES
(110001, 16.0, 30.0, 120.0, 0.0),
(110002, 20.0, 35.0, 90.0, 10.0),
(110400, 25.0, 40.0, 60.0, 15.0);

-- ---------------------------------------------------------------------
-- 8. Предметы PvP-контракта: honor-валюта и маркер War Mode.
--    entry 11201+ — вне диапазона 1200 предметов (10001..11200).
-- ---------------------------------------------------------------------
USE dm_items;
INSERT IGNORE INTO `item_template`
(`entry`,`class`,`subclass`,`name`,`displayid`,`quality`,`flags`,`BuyPrice`,`SellPrice`,`InventoryType`,`AllowableClass`,`AllowableRace`,`ItemLevel`,`StatsCount`,`bonding`)
VALUES
(11201, 4, 1, 'Badge of Black Matter (Honor Token)', 51201, 7, 0, 0, 0, 0, -1, -1, 95, 0, 0),
(11202, 4, 1, 'Duelist Signet (Quest Item)',         51202, 4, 0, 0, 0, 0, -1, -1, 10, 0, 0);

-- Миграция обратно совместима: IF NOT EXISTS / INSERT IGNORE.
