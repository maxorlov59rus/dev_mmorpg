-- =====================================================================
-- «Чёрная Материя» — fresh install DDL, база dm_auth
-- Аналог TrinityCore /sql/create/auth.sql + realmlist
-- MySQL 8.0, InnoDB, utf8mb4
-- =====================================================================

CREATE DATABASE IF NOT EXISTS dm_auth CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE dm_auth;

-- Таблица версии БД (как TRC auth.db_version): сервер refuse startup при mismatch
CREATE TABLE db_version (
  auth_version     SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  content_hash     CHAR(64)          NOT NULL DEFAULT '',   -- sha256 последнего применённого upgrade
  installed_at     TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;
INSERT INTO db_version VALUES (1, '', NOW());

CREATE TABLE accounts (
  id            INT UNSIGNED NOT NULL AUTO_INCREMENT,
  email         VARCHAR(255) NOT NULL,
  username      VARCHAR(32)  NOT NULL,             -- публичный логин
  flags         INT UNSIGNED NOT NULL DEFAULT 0,   -- биты: 1=suspended,2=verified,4=beta
  access_level  TINYINT UNSIGNED NOT NULL DEFAULT 0, -- 0 player,1 GM,2 admin,3 console (TRC-style)
  created_at    TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  last_login_at TIMESTAMP    NULL,
  last_ip       VARBINARY(16) NULL,
  PRIMARY KEY (id),
  UNIQUE KEY uk_email (email),
  UNIQUE KEY uk_username (username)
) ENGINE=InnoDB;

-- SRP6a verifier/salt (пароль никогда не хранится) — аналог TRC auth.account (s,a)
CREATE TABLE account_password (
  account_id   INT UNSIGNED NOT NULL,
  srp_salt     VARBINARY(64) NOT NULL,     -- 32-байтный salt
  srp_verifier VARBINARY(256) NOT NULL,    -- v = g^x mod N
  algo         TINYINT UNSIGNED NOT NULL DEFAULT 6, -- 6 = SRP6a-SHA512
  updated_at   TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (account_id),
  CONSTRAINT fk_pw_acc FOREIGN KEY (account_id) REFERENCES accounts(id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Короткоживущие токены шарда (realm_ticket) — выдаёт authserver, валидирует worldserver
CREATE TABLE session_tokens (
  token_id     CHAR(36) NOT NULL,           -- UUID
  account_id   INT UNSIGNED NOT NULL,
  shard_id     SMALLINT UNSIGNED NOT NULL,
  issued_at    BIGINT NOT NULL,             -- unix ts ms
  expires_at   BIGINT NOT NULL,             -- issued + 60s
  consumed     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (token_id),
  KEY idx_acc (account_id)
) ENGINE=InnoDB;

-- Реестр шардов (аналог TRC realmlist)
CREATE TABLE shards (
  id             SMALLINT UNSIGNED NOT NULL,
  name           VARCHAR(64) NOT NULL,
  region         ENUM('EU','US','SEA') NOT NULL,
  address        VARCHAR(64) NOT NULL,        -- host для world :8092
  population_cap INT UNSIGNED NOT NULL DEFAULT 15000,
  flags          TINYINT UNSIGNED NOT NULL DEFAULT 0, -- 1=test,2=pvp-focused,4=overloaded
  PRIMARY KEY (id)
) ENGINE=InnoDB;

-- Баны (три типа как в TRC: account / ip / hardware)
CREATE TABLE bans (
  id          INT UNSIGNED NOT NULL AUTO_INCREMENT,
  type        ENUM('account','ip','hardware') NOT NULL,
  target      VARCHAR(255) NOT NULL,        -- account_id / ip / hwid-hash
  reason      VARCHAR(255) NOT NULL DEFAULT '',
  banned_by   INT UNSIGNED NULL,            -- account id ГМ
  unban_time  BIGINT NULL,                  -- NULL = перманентный
  active      TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (id),
  KEY idx_target (type, target(64))
) ENGINE=InnoDB;

-- Правила автобана из античит-конвейера (Kafka consumer пишет вердикты)
CREATE TABLE auto_ban_rules (
  rule_id     SMALLINT UNSIGNED NOT NULL,
  metric      VARCHAR(64) NOT NULL,          -- e.g. 'speed_variance', 'gold_delta_hour'
  threshold   DOUBLE NOT NULL,
  action      ENUM('flag','mute','suspend_24h','ban') NOT NULL,
  enabled     TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (rule_id)
) ENGINE=InnoDB;

CREATE TABLE uptime (
  shard_id    SMALLINT UNSIGNED NOT NULL,
  start_time  BIGINT NOT NULL,
  end_time    BIGINT NULL,
  PRIMARY KEY (shard_id, start_time)
) ENGINE=InnoDB;
