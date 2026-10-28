-- =============================================================================
-- 04_dm_items.sql — база предметов (dm_items) для «Чёрной Материи»
-- Аналог TrinityCore world-домена предметов. Заполняется из
-- sql/upgrades/Insert_1200_Items.sql (генератор: tools/Generate_Item_Data.py).
-- =============================================================================
CREATE DATABASE IF NOT EXISTS `dm_items` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE `dm_items`;

DROP TABLE IF EXISTS `item_template`;
CREATE TABLE `item_template` (
  `entry`          INT UNSIGNED NOT NULL,
  `class`          TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '2=Weapon 4=Armor 1=Consumable 5=Trade 6=Quest',
  `subclass`       TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `name`           VARCHAR(255) NOT NULL,
  `displayid`      MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Клиентский ID -> mapping_displayid.csv',
  `quality`        TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0..5 poor..legendary',
  `flags`          INT UNSIGNED NOT NULL DEFAULT 0,
  `BuyPrice`       INT UNSIGNED NOT NULL DEFAULT 0,
  `SellPrice`      INT UNSIGNED NOT NULL DEFAULT 0,
  `InventoryType`  TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `AllowableClass`  INT NOT NULL DEFAULT -1,
  `AllowableRace`   INT NOT NULL DEFAULT -1,
  `ItemLevel`      SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  `durability`     SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  -- Для брони в dmg_min хранится ArmorValue (ilvl*1.2*slotMult), dmg_max/delay = 0
  `dmg_min`        INT UNSIGNED NOT NULL DEFAULT 0,
  `dmg_max`        INT UNSIGNED NOT NULL DEFAULT 0,
  `delay`          INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'скорость оружия, мс',
  `stat_type1`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value1`    INT NOT NULL DEFAULT 0,
  `stat_type2`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value2`    INT NOT NULL DEFAULT 0,
  `stat_type3`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value3`    INT NOT NULL DEFAULT 0,
  `stat_type4`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value4`    INT NOT NULL DEFAULT 0,
  `stat_type5`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value5`    INT NOT NULL DEFAULT 0,
  `stat_type6`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value6`    INT NOT NULL DEFAULT 0,
  `stat_type7`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value7`    INT NOT NULL DEFAULT 0,
  `stat_type8`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value8`    INT NOT NULL DEFAULT 0,
  `stat_type9`     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value9`    INT NOT NULL DEFAULT 0,
  `stat_type10`    TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `stat_value10`   INT NOT NULL DEFAULT 0,
  `socket_count`   TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `spellid_1`      MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  `spelltrigger_1` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=use 1=PROC(on hit; epic weapons)',
  PRIMARY KEY (`entry`),
  KEY `idx_class_quality` (`class`, `quality`),
  KEY `idx_ilvl` (`ItemLevel`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Соответствие серверный ItemID -> клиентский ассет UE5 (загружается как dbc-подобная таблица)
DROP TABLE IF EXISTS `displayid_map`;
CREATE TABLE `displayid_map` (
  `item_id`    INT UNSIGNED NOT NULL,
  `display_id` MEDIUMINT UNSIGNED NOT NULL,
  `mesh_path`  VARCHAR(255) NOT NULL DEFAULT '',
  `icon_path`  VARCHAR(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`item_id`),
  KEY `idx_display` (`display_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Enchant-«подложки» Niagara (см. EquipmentManagerComponent.cpp)
DROP TABLE IF EXISTS `enchant_effects`;
CREATE TABLE `enchant_effects` (
  `enchant_id`     SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `element`        ENUM('fire','ice','holy','shade','storm') NOT NULL,
  `niagara_system` VARCHAR(255) NOT NULL,
  `socket_name`    VARCHAR(64) NOT NULL DEFAULT 'weapon_socket',
  `min_quality`    TINYINT UNSIGNED NOT NULL DEFAULT 5,
  PRIMARY KEY (`enchant_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO `enchant_effects` (`element`,`niagara_system`,`socket_name`,`min_quality`) VALUES
('fire', '/Game/VFX/Enchants/NS_Enchant_Fire.NS_Enchant_Fire', 'weapon_socket', 5),
('ice',  '/Game/VFX/Enchants/NS_Enchant_Ice.NS_Enchant_Ice',   'weapon_socket', 5),
('holy', '/Game/VFX/Enchants/NS_Enchant_Holy.NS_Enchant_Holy', 'shield_socket', 5),
('shade','/Game/VFX/Enchants/NS_Enchant_Shade.NS_Enchant_Shade','weapon_socket',5),
('storm','/Game/VFX/Enchants/NS_Enchant_Storm.NS_Enchant_Storm','weapon_socket',4);
