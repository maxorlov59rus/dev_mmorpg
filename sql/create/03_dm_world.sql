-- =====================================================================
-- «Чёрная Материя» — fresh install DDL, база dm_world
-- Аналог TrinityCore /sql/create/world.sql + фракционные таблицы (frontline/seepage/season)
-- MySQL 8.0, InnoDB, utf8mb4
-- =====================================================================

CREATE DATABASE IF NOT EXISTS dm_world CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE dm_world;

CREATE TABLE db_version (
  world_version SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  content_hash  CHAR(64)          NOT NULL DEFAULT '',
  installed_at  TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;
INSERT INTO db_version VALUES (1, '', NOW());

-- ---------- спавн и состояние мира (как TRC creature/gameobject_respawn) ----------
CREATE TABLE creature_respawn (
  guid        INT UNSIGNED NOT NULL PRIMARY KEY,   -- static spawn id из dbc карты
  map_id      SMALLINT UNSIGNED NOT NULL,
  respawn_ms  BIGINT NOT NULL,                     -- абсолютное время появления
  instance_id INT UNSIGNED NOT NULL DEFAULT 0,
  killed_by_json JSON NULL,                        -- аудит (кто/чем убил для аналитики)
  INDEX idx_map_time (map_id, instance_id, respawn_ms)
) ENGINE=InnoDB;

CREATE TABLE gameobject_respawn (
  guid INT UNSIGNED NOT NULL, map_id SMALLINT UNSIGNED NOT NULL,
  state TINYINT NOT NULL DEFAULT 0,                -- 0 usable, 1 used (gate/farm cooldown)
  respawn_ms BIGINT NOT NULL, instance_id INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (guid, map_id), INDEX idx_time (respawn_ms)
) ENGINE=InnoDB;

CREATE TABLE area_trigger_state (     -- зоны скриптов: входе/выходе события квестов
  trigger_id MEDIUMINT UNSIGNED NOT NULL, guid INT UNSIGNED NOT NULL,
  entered_ms BIGINT NOT NULL, PRIMARY KEY (trigger_id, guid)
) ENGINE=InnoDB;

CREATE TABLE weather (                -- как TRC weather: влияет на Seepage-атмосферу
  map_id SMALLINT UNSIGNED NOT NULL, zone_id SMALLINT UNSIGNED NOT NULL,
  type TINYINT UNSIGNED NOT NULL DEFAULT 0, intensity_fixed INT NOT NULL DEFAULT 0,
  PRIMARY KEY (map_id, zone_id)
) ENGINE=InnoDB;

CREATE TABLE world_state (            -- глобальные флаги шарда (аналог TRC world_state)
  id SMALLINT UNSIGNED NOT NULL PRIMARY KEY, `value` BIGINT NOT NULL, comment VARCHAR(96) NULL
) ENGINE=InnoDB;

-- ---------- ЧЁРНАЯ МАТЕРИЯ: Истечения (Seepage) ----------
CREATE TABLE seepage_template (       -- статика: где могут возникать Истечения
  zone_id SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
  base_weight INT UNSIGNED NOT NULL DEFAULT 100,   -- склонность к прорыву
  max_concurrent TINYINT UNSIGNED NOT NULL DEFAULT 2
) ENGINE=InnoDB;

CREATE TABLE seepage_state (          -- активные инстансы: планировщик worldserver/Faction
  id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  zone_id SMALLINT UNSIGNED NOT NULL,
  started_ms BIGINT NOT NULL, ends_ms BIGINT NOT NULL,
  intensity TINYINT UNSIGNED NOT NULL DEFAULT 1,   -- 1..5 тиров
  hollowed_alive SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  active TINYINT UNSIGNED NOT NULL DEFAULT 1,
  INDEX idx_zone_active (zone_id, active)
) ENGINE=InnoDB;

-- «Грех» зоны — накопительный драйвер появления Seepage (PvP-убийства/смерти)
CREATE TABLE zone_sin (
  zone_id SMALLINT UNSIGNED NOT NULL, week_epoch INT UNSIGNED NOT NULL,
  pvp_kills INT UNSIGNED NOT NULL DEFAULT 0, deaths_in_seepage INT UNSIGNED NOT NULL DEFAULT 0,
  umbra_casts INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (zone_id, week_epoch)
) ENGINE=InnoDB;

-- ---------- ФРАКЦИИ / FRONTLINE ----------
CREATE TABLE faction_def (
  id TINYINT UNSIGNED NOT NULL PRIMARY KEY, label_ru VARCHAR(64), label_en VARCHAR(64),
  capital_map SMALLINT UNSIGNED NOT NULL
) ENGINE=InnoDB;
INSERT INTO faction_def VALUES
 (1,'Орден Рассвета','Covenant of Dawn',1100),
 (2,'Круг Поглощения','Circle of the Hollow',1200),
 (3,'Гильдия Порога','Threshold Guilds',1300);

CREATE TABLE frontline_zone (
  id TINYINT UNSIGNED NOT NULL PRIMARY KEY, name VARCHAR(64) NOT NULL,
  map_id SMALLINT UNSIGNED NOT NULL, season_score_mult_fixed INT NOT NULL DEFAULT 65536 -- Q16.16 x1.0
) ENGINE=InnoDB;

CREATE TABLE frontline_point (
  id SMALLINT UNSIGNED NOT NULL PRIMARY KEY, zone_id TINYINT UNSIGNED NOT NULL,
  name VARCHAR(64) NOT NULL, pos_x INT NOT NULL, pos_y INT NOT NULL, pos_z INT NOT NULL,
  capture_hold_sec SMALLINT UNSIGNED NOT NULL DEFAULT 300
) ENGINE=InnoDB;

-- Текущий контроль точки (переживает рестарт worldserver'а)
CREATE TABLE frontline_control (
  point_id SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
  owner_faction TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- 0 = спорно/ничейно
  last_capture_ms BIGINT NULL,
  holder_score_fixed INT NOT NULL DEFAULT 0           -- прогресс удержания текущим владельцем
) ENGINE=InnoDB;

-- ---------- СЕЗОНЫ ----------
CREATE TABLE season (
  id SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
  starts_ms BIGINT NOT NULL, ends_ms BIGINT NOT NULL, theme VARCHAR(64) NOT NULL,
  status ENUM('planned','live','finished') NOT NULL DEFAULT 'planned'
) ENGINE=InnoDB;

CREATE TABLE season_results (
  season_id SMALLINT UNSIGNED NOT NULL, faction TINYINT UNSIGNED NOT NULL,
  score BIGINT NOT NULL DEFAULT 0, rank TINYINT UNSIGNED NULL,
  PRIMARY KEY (season_id, faction)
) ENGINE=InnoDB;

-- ---------- АУКЦИОН / ПОЧТА-ЭКОНОМИКА (персист рынка) ----------
CREATE TABLE auction_items (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  entry_guid BIGINT UNSIGNED NOT NULL, seller_guid INT UNSIGNED NOT NULL,
  buy_price BIGINT NOT NULL, bid_price BIGINT NOT NULL DEFAULT 0,
  bidder_guid INT UNSIGNED NULL, ends_ms BIGINT NOT NULL,
  INDEX idx_expire (ends_ms), INDEX idx_seller (seller_guid)
) ENGINE=InnoDB;

-- ---------- ИНСТАНСЫ ----------
CREATE TABLE instance_locks (         -- TRC-style raid lockout per group/leader
  id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  instance_template_id MEDIUMINT UNSIGNED NOT NULL, difficulty TINYINT UNSIGNED NOT NULL,
  leader_guid INT UNSIGNED NOT NULL, reset_ms BIGINT NOT NULL, extended TINYINT NOT NULL DEFAULT 0,
  members_json JSON NOT NULL, UNIQUE KEY uk_lock (leader_guid, instance_template_id, difficulty)
) ENGINE=InnoDB;

CREATE TABLE instance_snapshots (     -- страховка от падения mapserver: состояние боссов
  instance_id INT UNSIGNED NOT NULL PRIMARY KEY,
  map_id SMALLINT UNSIGNED NOT NULL, saved_ms BIGINT NOT NULL, state_json JSON NOT NULL
) ENGINE=InnoDB;

-- ---------- СЛОИ ЗОН (layering при перенаселении) ----------
CREATE TABLE zone_layers (
  layer_id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  zone_id SMALLINT UNSIGNED NOT NULL, mapserver_addr VARCHAR(64) NOT NULL,
  pop INT UNSIGNED NOT NULL DEFAULT 0, cap INT UNSIGNED NOT NULL DEFAULT 300,
  created_ms BIGINT NOT NULL, INDEX idx_zone (zone_id)
) ENGINE=InnoDB;

-- ---------- Динамические события / античит-журнал ----------
CREATE TABLE dynamic_events_log (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  event_type VARCHAR(32) NOT NULL, zone_id SMALLINT UNSIGNED NULL,
  data_json JSON NOT NULL, created_ms BIGINT NOT NULL, INDEX idx_type_time (event_type, created_ms)
) ENGINE=InnoDB;

-- ---------- Hotfixes (аналог TRC hotfixes DB) ----------
CREATE TABLE hotfixes (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  table_name VARCHAR(64) NOT NULL, row_id BIGINT NOT NULL,
  payload BLOB NOT NULL, checksum CHAR(64) NOT NULL, applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  UNIQUE KEY uk_row (table_name, row_id)
) ENGINE=InnoDB;
