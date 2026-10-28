-- =====================================================================
-- «Чёрная Материя» — fresh install DDL, база dm_characters
-- Аналог TrinityCore /sql/create/characters.sql (+ фракционные расширения)
-- MySQL 8.0, InnoDB, utf8mb4
-- =====================================================================

CREATE DATABASE IF NOT EXISTS dm_characters CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE dm_characters;

CREATE TABLE db_version (
  characters_version SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  content_hash       CHAR(64)          NOT NULL DEFAULT '',
  installed_at       TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;
INSERT INTO db_version VALUES (1, '', NOW());

-- faction: 0=нейтральный, 1=Орден Рассвета, 2=Круг Поглощения, 3=Гильдия Порога
-- class:   1=Воин 2=Жрец 3=Маг 4=Плут 5=Охотник
CREATE TABLE characters (
  guid        INT UNSIGNED NOT NULL AUTO_INCREMENT,
  account_id  INT UNSIGNED NOT NULL,           -- логическая FK -> dm_auth.accounts.id
  name        VARCHAR(24)  NOT NULL,
  race        TINYINT UNSIGNED NOT NULL,
  class       TINYINT UNSIGNED NOT NULL,
  faction     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  level       TINYINT UNSIGNED NOT NULL DEFAULT 1,
  xp          BIGINT UNSIGNED NOT NULL DEFAULT 0,
  gender      TINYINT UNSIGNED NOT NULL DEFAULT 0,
  map_id      SMALLINT UNSIGNED NOT NULL DEFAULT 1000,  -- стартовая зона по фракции
  zone_id     SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  pos_x INT NOT NULL, pos_y INT NOT NULL, pos_z INT NOT NULL, -- fixed Q16.16
  orientation INT NOT NULL,                    -- fixed Q16.16 radiant
  creation_epoch BIGINT NOT NULL,              -- ms, для квоты смены фракции (60 дней)
  online      TINYINT UNSIGNED NOT NULL DEFAULT 0,
  deleted_at  BIGINT NULL,                     -- soft delete (восстановление 30 дней)
  PRIMARY KEY (guid),
  UNIQUE KEY uk_name (name),
  KEY idx_account (account_id)
) ENGINE=InnoDB;

CREATE TABLE char_stats (
  guid     INT UNSIGNED NOT NULL PRIMARY KEY,
  str INT UNSIGNED, agi INT UNSIGNED, `int` INT UNSIGNED, vit INT UNSIGNED, spi INT UNSIGNED,
  stat_free TINYINT UNSIGNED NOT NULL DEFAULT 0,     -- нераспределённые очки
  hp_max INT UNSIGNED NOT NULL, hp_cur INT UNSIGNED NOT NULL,
  mana_max INT UNSIGNED NOT NULL, mana_cur INT UNSIGNED NOT NULL, -- Focus как отдельный ресурс ниже
  item_level SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  CONSTRAINT fk_stat FOREIGN KEY (guid) REFERENCES characters(guid) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Corruption (Порча Чёрной Материей): скрытый счётчик + флаг Изменённого
CREATE TABLE corruption (
  guid        INT UNSIGNED NOT NULL PRIMARY KEY,
  value       SMALLINT UNSIGNED NOT NULL DEFAULT 0,  -- 0..10000 (=0..100.00%)
  altered     TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- >=70% и принят обряд => 1
  last_event  VARCHAR(64) NULL,                      -- причина последнего изменения (аудит)
  CONSTRAINT fk_corp FOREIGN KEY (guid) REFERENCES characters(guid) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Фракционные валюты: Lumen / Umbra / Splinters + Gold + Insight(account-уровень в billing)
CREATE TABLE currencies (
  guid    INT UNSIGNED NOT NULL,
  type    ENUM('gold','lumen','umbra','splinters','insight') NOT NULL,
  amount  BIGINT NOT NULL DEFAULT 0,
  PRIMARY KEY (guid, type),
  CONSTRAINT fk_cur FOREIGN KEY (guid) REFERENCES characters(guid) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE char_skills (
  guid INT UNSIGNED NOT NULL, skill_id MEDIUMINT UNSIGNED NOT NULL,
  value SMALLINT UNSIGNED NOT NULL, PRIMARY KEY (guid, skill_id)
) ENGINE=InnoDB;

CREATE TABLE char_spells (
  guid INT UNSIGNED NOT NULL, spell_id MEDIUMINT UNSIGNED NOT NULL,
  rank TINYINT UNSIGNED NOT NULL DEFAULT 1, source TINYINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (guid, spell_id)
) ENGINE=InnoDB;

-- Активные ауры переживают логбаут (важно для DoT через Истечение)
CREATE TABLE char_auras (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  guid INT UNSIGNED NOT NULL,
  spell_id MEDIUMINT UNSIGNED NOT NULL, effect_idx TINYINT UNSIGNED NOT NULL DEFAULT 0,
  stacks TINYINT UNSIGNED NOT NULL DEFAULT 1,
  expires_ms BIGINT NULL,                     -- абсолютный unix-ms или NULL (до выгрузки)
  caster_guid INT UNSIGNED NULL,
  INDEX idx_guid (guid)
) ENGINE=InnoDB;

CREATE TABLE cooldowns (
  guid INT UNSIGNED NOT NULL, cd_group MEDIUMINT UNSIGNED NOT NULL,
  ready_ms BIGINT NOT NULL, PRIMARY KEY (guid, cd_group)
) ENGINE=InnoDB;

-- Инвентарь: контейнеры + предметы (нормализация вместо blob-JSON, как TRC character_inventory)
CREATE TABLE inventory_buckets (
  bucket_id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  owner_guid INT UNSIGNED NOT NULL,
  kind ENUM('bag','bank','guild_bank','trade_pending') NOT NULL,
  guild_id INT UNSIGNED NULL, slot_count TINYINT UNSIGNED NOT NULL DEFAULT 20,
  INDEX idx_owner (owner_guid)
) ENGINE=InnoDB;

CREATE TABLE items (
  entry_guid BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, -- уникальный инстанс предмета
  item_id    MEDIUMINT UNSIGNED NOT NULL,          -- dbc entry
  owner_bucket INT UNSIGNED NULL, slot TINYINT UNSIGNED NULL,
  quantity INT UNSIGNED NOT NULL DEFAULT 1,
  quality_roll TINYINT UNSIGNED NOT NULL DEFAULT 0, -- craft quality tier
  enchant_json JSON NULL,                          -- только неигровые метаданные!
  lootable_by INT UNSIGNED NULL,                   -- temp loot state
  INDEX idx_bucket (owner_bucket, slot)
) ENGINE=InnoDB;

CREATE TABLE equipment (
  guid INT UNSIGNED NOT NULL, equip_slot TINYINT UNSIGNED NOT NULL,
  entry_guid BIGINT UNSIGNED NOT NULL,
  PRIMARY KEY (guid, equip_slot)
) ENGINE=InnoDB;

CREATE TABLE quest_status (
  guid INT UNSIGNED NOT NULL, quest_id MEDIUMINT UNSIGNED NOT NULL,
  state ENUM('accepted','complete_ready','rewarded_failed_log') NOT NULL,
  timer_ms BIGINT NULL, objectives_json JSON NOT NULL,
  PRIMARY KEY (guid, quest_id)
) ENGINE=InnoDB;

CREATE TABLE reputation (
  guid INT UNSIGNED NOT NULL, faction_id MEDIUMINT UNSIGNED NOT NULL,
  standing INT NOT NULL DEFAULT 0,               -- fixed-point, threshold в dbc
  PRIMARY KEY (guid, faction_id)
) ENGINE=InnoDB;

CREATE TABLE titles (
  guid INT UNSIGNED NOT NULL, title_id SMALLINT UNSIGNED NOT NULL,
  active TINYINT UNSIGNED NOT NULL DEFAULT 0, earned_ms BIGINT NOT NULL,
  PRIMARY KEY (guid, title_id)
) ENGINE=InnoDB;

-- Настройки клиента хранятся сервером => кроссплатформенный перенос PC↔Android
CREATE TABLE settings (
  guid INT UNSIGNED NOT NULL, section VARCHAR(32) NOT NULL, payload JSON NOT NULL,
  PRIMARY KEY (guid, section)
) ENGINE=InnoDB;

CREATE TABLE mail (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  receiver_guid INT UNSIGNED NOT NULL, sender_guid INT UNSIGNED NULL,
  subject VARCHAR(96) NOT NULL, body TEXT NULL,
  money_gold BIGINT NOT NULL DEFAULT 0, unread TINYINT UNSIGNED NOT NULL DEFAULT 1,
  created_ms BIGINT NOT NULL, expire_ms BIGINT NOT NULL,
  INDEX idx_recv (receiver_guid, unread)
) ENGINE=InnoDB;

-- Гильдии
CREATE TABLE guilds (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(24) NOT NULL, faction TINYINT UNSIGNED NOT NULL,
  master_guid INT UNSIGNED NOT NULL, created_ms BIGINT NOT NULL,
  bank_gold BIGINT NOT NULL DEFAULT 0, emblem_json JSON NULL,
  UNIQUE KEY uk_name (name)
) ENGINE=InnoDB;

CREATE TABLE guild_ranks (
  guild_id INT UNSIGNED NOT NULL, rank_id TINYINT UNSIGNED NOT NULL,
  label VARCHAR(16) NOT NULL, perms INT UNSIGNED NOT NULL,  -- bitmask прав
  PRIMARY KEY (guild_id, rank_id)
) ENGINE=InnoDB;

CREATE TABLE guild_members (
  guid INT UNSIGNED NOT NULL PRIMARY KEY,
  guild_id INT UNSIGNED NOT NULL, rank_id TINYINT UNSIGNED NOT NULL,
  joined_ms BIGINT NOT NULL, note VARCHAR(48) NULL,
  INDEX idx_guild (guild_id)
) ENGINE=InnoDB;

-- Группы/рейды живут в памяти worldserver, но lockout'ы и составы — здесь
CREATE TABLE raid_resets (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  leader_guid INT UNSIGNED NOT NULL, instance_template_id MEDIUMINT UNSIGNED NOT NULL,
  difficulty TINYINT UNSIGNED NOT NULL, reset_ms BIGINT NOT NULL,
  members_json JSON NOT NULL
) ENGINE=InnoDB;

-- SFX: очередь сохранения (dirty-флаги как TRC SaveToDB) обрабатываются асинхронно
CREATE TABLE save_queue (
  guid INT UNSIGNED NOT NULL PRIMARY KEY, dirty_mask INT UNSIGNED NOT NULL,
  queued_ms BIGINT NOT NULL
) ENGINE=InnoDB;
