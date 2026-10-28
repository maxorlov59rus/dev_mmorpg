-- =====================================================================
-- «Чёрная Материя» — DB_Schema.sql (миграция v1 -> v2)
-- Ядро фракционной системы: репутация, ренегаты, квесты, лут, условия.
--
-- Стиль TrinityCore: отдельные CREATE-файлы для fresh install + upgrade
-- скрипты в sql/upgrades/. Этот файл = ПОВТОРНО БЕЗОПАСНАЯ миграция
-- (information_schema guard'ы), применяется scripts/db/apply_migrations.sh
-- (аналог TRC /sql/updates runner). MySQL 8.0.
--
-- Порядок применения: сначала этот файл, затем StarterQuests.sql, NPC_Config.sql.
-- =====================================================================

USE dm_characters;

-- ---------- characters: денормализованные горячие поля ----------
-- standing по всем фракциям живёт в таблице reputation (уже существует);
-- эти колонки — materialized view для быстрого WHERE при PvP-матчинге и
-- загрузки персона без JOIN (инвариант поддерживается ReputationMgr::SaveToDB).
ALTER TABLE characters
  ADD COLUMN reputation_faction_1 INT NOT NULL DEFAULT 0
      COMMENT 'Q16 fixed standing, Стражи Света (mirrors reputation.faction_id=1)',
  ADD COLUMN reputation_faction_2 INT NOT NULL DEFAULT 0
      COMMENT 'Q16 fixed standing, Падшие (mirrors reputation.faction_id=2)',
  ADD COLUMN is_renegade TINYINT UNSIGNED NOT NULL DEFAULT 0
      COMMENT 'производный флаг: repA < -5000 AND repB < 0 (ReputationMgr)',
  ADD COLUMN at_war_with_all TINYINT UNSIGNED NOT NULL DEFAULT 0
      COMMENT 'PvP-режим изгоя: все игроки/NPC враждебны, города закрыты';

-- Индекс под подбор целей PvP/AOI (renegades редки — partial index через generated col)
CREATE INDEX idx_renegade ON characters (is_renegade) ;

-- Backfill из таблицы reputation (на случай, если данные уже были):
UPDATE characters c
  JOIN reputation r ON r.guid = c.guid AND r.faction_id = 1
  SET c.reputation_faction_1 = r.standing;
UPDATE characters c
  JOIN reputation r ON r.guid = c.guid AND r.faction_id = 2
  SET c.reputation_faction_2 = r.standing;

-- Пересчёт флага после backfill (то же правило, что RecalcRenegateFlag):
UPDATE characters
  SET is_renegade = 1, at_war_with_all = 1
  WHERE reputation_faction_1 < -5000 AND reputation_faction_2 < 0;

-- Таблица reputation: расширяем семантику faction_id (4 = служебная фракция
-- изгоев FACTION_RENEGADE) + аудит последней причины изменения.
ALTER TABLE reputation
  ADD COLUMN change_reason VARCHAR(32) NULL AFTER standing;

-- ---------- Стартовый инвентарь зависит от фракции: справочник ----------
CREATE TABLE IF NOT EXISTS starting_outfit (
  faction TINYINT UNSIGNED NOT NULL,
  class   TINYINT UNSIGNED NOT NULL,
  race    TINYINT UNSIGNED NOT NULL,
  items_json JSON NOT NULL,          -- [{entry,count},{...}] -> выдаёт CharacterCreateHandler
  PRIMARY KEY (faction, class, race)
) ENGINE=InnoDB;

-- =====================================================================
-- dm_world: конфиг контента
-- =====================================================================
USE dm_world;

-- ---------- creature_template (+ фракционные флаги) ----------
-- Аналог TRC creature_template: UnitFlags (bitmask aggressiveness), FactionAlliance/AH.
-- FactionAlliance: 0x0001 Альянс Света, 0x0002 Орден Пепла/Круг Поглощения,
--                  0x0004 нейтральные гильдии, 0x0008 ренегаты (служебные NPC)
CREATE TABLE IF NOT EXISTS creature_template (
  entry        MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  name         VARCHAR(64) NOT NULL,
  subname      VARCHAR(64) NULL,
  min_level    TINYINT UNSIGNED NOT NULL DEFAULT 1,
  max_level    TINYINT UNSIGNED NOT NULL DEFAULT 1,
  faction_A    SMALLINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'альянсовая фракция (стражи/городские NPC)',
  faction_H    SMALLINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'horde-аналог: вторая фракция (Падшие)',
  -- unit_flags: как TRC (USE_FLAGS = 0x??); бит 21 = ATTACK_RENEGADE_ON_SIGHT
  unit_flags   INT UNSIGNED NOT NULL DEFAULT 0,
  unit_flags2  INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'бит21=AttackRenegade; бит7=Passive',
  type_flags   INT UNSIGNED NOT NULL DEFAULT 0,
  damage_mult  DECIMAL(6,3) NOT NULL DEFAULT 1.000,
  health_mult  DECIMAL(6,3) NOT NULL DEFAULT 1.000,
  lootid       MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'FK reference_loot_template.entry',
  pickpocketloot MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  script_name  VARCHAR(64) NOT NULL DEFAULT '',
  KEY idx_factions (faction_A, faction_H)
) ENGINE=InnoDB;

-- ---------- Квесты (структура как TRC quest_template + наши поля) ----------
CREATE TABLE IF NOT EXISTS quest_template (
  ID            MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  QuestType     TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 default,1 group...',
  ReqMinFactionValue SMALLINT NOT NULL DEFAULT 0,
  Flags         INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'бит0 hidden(auto-start),бит1 no-completion-log...',
  SpecialFlags  SMALLINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'бит4=autostart,бит9=repeatable_daily',
  Title         VARCHAR(128) NOT NULL,
  Details       TEXT NULL,
  Objectives    TEXT NULL,
  OfferRewardText TEXT NULL,
  RequestItemsText TEXT NULL,
  CompleteText  TEXT NULL,
  Reward        INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'money copper',
  RewardMailTemplateID MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  RewRepFaction1   SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  RewRepValue1     INT NOT NULL DEFAULT 0 COMMENT 'raw Q16 units (250 очков = 4000)',
  RewRepFaction2   SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  RewRepValue2     INT NOT NULL DEFAULT 0,
  RewItemId1       MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  RewItemCount1    SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  RewItemId2       MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  RewItemCount2    SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  RequiredNpcOrGo1 MEDIUMINT NOT NULL DEFAULT 0,
  RequiredNpcOrGoCount1 SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  RequiredCreatureKill1 MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  RequiredCreatureKillCount1 SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  StartEvent     VARCHAR(64) NOT NULL DEFAULT '' COMMENT 'e.g. on_renegade_gained (для q_1003)',
  IsHidden       TINYINT UNSIGNED NOT NULL DEFAULT 0,
  MinLevel       TINYINT UNSIGNED NOT NULL DEFAULT 1,
  ZoneId         SMALLINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- ---------- Gossip (диалоги NPC с условиями репутации) ----------
CREATE TABLE IF NOT EXISTS gossip_menu (
  ID        MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  TextID    MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  LanguageID TINYINT UNSIGNED NOT NULL DEFAULT 0,
  ScriptName VARCHAR(64) NOT NULL DEFAULT ''
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS gossip_menu_option (
  MenuID      MEDIUMINT UNSIGNED NOT NULL,
  ID          SMALLINT UNSIGNED NOT NULL,
  OptionIcon  TINYINT UNSIGNED NOT NULL DEFAULT 0,
  OptionText  TEXT NOT NULL,
  OptionBroadcastTextID MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  OptionType  TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 talk,1 quest,2 trade,...',
  Action     MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  ActionMenuID MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  ActionPoiID  MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  BoxCoded     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  -- >>> расширение по ТЗ: condition_id фильтрует показ опции
  --     (0 = всегда; см. DM_Conditions ниже — репутация > Friendly и т.п.)
  condition_id INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (MenuID, ID)
) ENGINE=InnoDB;

-- Справочник условий (аналог TRC conditions system, упрощённый до ядра):
CREATE TABLE IF NOT EXISTS dm_conditions (
  id           INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  kind         ENUM('rep_rank_min','rep_rank_max','is_renegade','not_renegade','quest_active','faction_start') NOT NULL,
  param1       INT NOT NULL DEFAULT 0 COMMENT 'kind-dependent: faction_id / rank enum / quest id',
  param2       INT NOT NULL DEFAULT 0 COMMENT 'rank threshold (RANK_FRIENDLY=4) и т.п.',
  comment      VARCHAR(128) NULL
) ENGINE=InnoDB;

INSERT INTO dm_conditions (kind, param1, param2, comment) VALUES
 ('rep_rank_min', 1, 4, 'Репутация со Стражами Света >= FRIENDLY'),
 ('rep_rank_min', 2, 4, 'Репутация с Падшими >= FRIENDLY'),
 ('is_renegade',  0, 0, 'Только для изгоев (черный рынок)'),
 ('not_renegade', 0, 0, 'Не для изгоев (обычные ветки диалога)');

-- Runtime-view для серверного кэша (SmartAI читает через LoadGossipData):
CREATE OR REPLACE VIEW gossip_option_visible AS
SELECT gmo.MenuID, gmo.ID, gmo.OptionText, gmo.OptionType, gmo.Action,
       gmo.condition_id, dc.kind AS cond_kind, dc.param1 AS cond_faction,
       dc.param2 AS cond_threshold
FROM gossip_menu_option gmo
LEFT JOIN dm_conditions dc ON dc.id = gmo.condition_id;

-- ---------- Предметы ----------
CREATE TABLE IF NOT EXISTS item_template (
  entry        MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  class        TINYINT UNSIGNED NOT NULL,
  subclass     TINYINT UNSIGNED NOT NULL,
  name         VARCHAR(64) NOT NULL,
  displayid    INT UNSIGNED NOT NULL DEFAULT 0,
  Quality      TINYINT UNSIGNED NOT NULL DEFAULT 1,
  ItemLevel    SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  RequiredLevel TINYINT UNSIGNED NOT NULL DEFAULT 1,
  -- >>> по ТЗ: класс/расовые ограничения bitmask (как TRC AllowableClass/Race)
  AllowableClass INT   NOT NULL DEFAULT -1 COMMENT '-1=all; bitmask class ids',
  AllowableRace  INT   NOT NULL DEFAULT -1 COMMENT '-1=all; bitmask race ids',
  Bonding      TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 none,1 BoP,2 BoE,3 quest-item,4 inline-BoP',
  -- >>> фракционная привязка: требует standing >= threshold для equip/buy
  FactionRelated TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0=no; 1..4 faction id',
  FactionReqRank TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'min RepRank enum to acquire/equip',
  BuyCount     TINYINT UNSIGNED NOT NULL DEFAULT 1,
  BuyPrice     INT UNSIGNED NOT NULL DEFAULT 0,
  SellPrice    INT UNSIGNED NOT NULL DEFAULT 0,
  Material     TINYINT NOT NULL DEFAULT 0 COMMENT '1 cloth,2 leather,3 mail,4 plate (стартовые сеты)',
  KEY idx_faction (FactionRelated, FactionReqRank)
) ENGINE=InnoDB;

-- ---------- Лут: reference-таблицы фракционных зон ----------
-- Как TRC reference_loot_template + our RuleMask для zone-faction override.
CREATE TABLE IF NOT EXISTS reference_loot_template (
  Entry     MEDIUMINT UNSIGNED NOT NULL COMMENT 'loot group id',
  Item      MEDIUMINT UNSIGNED NOT NULL,
  Reference MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  Chance    FLOAT NOT NULL DEFAULT 0 COMMENT 'percent x100 (TRC style)',
  QuestRequired TINYINT UNSIGNED NOT NULL DEFAULT 0,
  LootMode  TINYINT UNSIGNED NOT NULL DEFAULT 0,
  GroupId   TINYINT UNSIGNED NOT NULL DEFAULT 0,
  MinCount  SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  MaxCount  SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  Comment   VARCHAR(128) NULL,
  PRIMARY KEY (Entry, Item)
) ENGINE=InnoDB;

-- Маппинг «зона -> активная фракция контроля -> какая loot-группа применяется».
-- Позволяет менять дроп мобов в зависимости от текущего владельца frontline-зоны
-- (dm_world.frontline_control читается LootMgr при roll'е).
CREATE TABLE IF NOT EXISTS zone_loot_rule (
  zone_id       SMALLINT UNSIGNED NOT NULL,
  owner_faction TINYINT UNSIGNED NOT NULL COMMENT '0=ничейно, 1..3 владелец точки',
  loot_group    MEDIUMINT UNSIGNED NOT NULL COMMENT '-> reference_loot_template.Entry',
  PRIMARY KEY (zone_id, owner_faction)
) ENGINE=InnoDB;

-- ---------- Ресурсы/майннинг: флаг DropFaction + поправка скорости ----------
CREATE TABLE IF NOT EXISTS mining_node_def (
  node_entry   MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY COMMENT 'gameobject entry',
  item_entry   MEDIUMINT UNSIGNED NOT NULL COMMENT 'что добывается (5001=Этерниум)',
  drop_faction_mask TINYINT UNSIGNED NOT NULL DEFAULT 7 COMMENT 'биты фракций с бонусом; ренегат=бит4 не выставлен => штраф',
  renegade_speed_pct_x100 TINYINT UNSIGNED NOT NULL DEFAULT 50 COMMENT '50 = ренегат копает на 50% (проверка Player::UpdateSkills/Mining)',
  min_skill    SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  respawn_ms   BIGINT NOT NULL DEFAULT 300000
) ENGINE=InnoDB;

-- Версия БД (drift-detector CI сверяет с этим числом)
UPDATE db_version SET world_version = 2 WHERE world_version < 2;
USE dm_characters;
UPDATE db_version SET characters_version = 2 WHERE characters_version < 2;
