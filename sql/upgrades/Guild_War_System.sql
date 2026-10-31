-- ============================================================================
-- Guild_War_System.sql — Система войны гильдий
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ----------------------------------------------------------------------------
-- 1. GUILD_WARS — Активные войны между гильдиями
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_wars (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    guild_a INT UNSIGNED NOT NULL, -- ID гильдии атакующей
    guild_b INT UNSIGNED NOT NULL, -- ID гильдии защиты
    war_state TINYINT UNSIGNED NOT NULL DEFAULT 0, -- 0=мир, 1=война, 2=победа
    start_time INT UNSIGNED NOT NULL, -- Начало войны (Unix timestamp)
    end_time INT UNSIGNED NOT NULL DEFAULT 0, -- Конец войны (Unix timestamp)
    declared_at INT UNSIGNED NOT NULL, -- Когда была объявлена война
    reason TEXT NOT NULL, -- Причина войны
    peace_treaty_until INT UNSIGNED NOT NULL DEFAULT 0, -- До когда мир (Unix timestamp)
    contribution_a INT UNSIGNED NOT NULL DEFAULT 0, -- Вклад атакующей гильдии
    contribution_b INT UNSIGNED NOT NULL DEFAULT 0, -- Вклад оборонительной гильдии
    UNIQUE KEY unique_war_pair (guild_a, guild_b)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 2. GUILD_WAR_PARTICIPANTS — Участники войны
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_war_participants (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    war_id INT UNSIGNED NOT NULL,
    player_guid BIGINT UNSIGNED NOT NULL,
    faction TINYINT UNSIGNED NOT NULL, -- 0=не участвует, 1=атака, 2=оборона
    kills INT UNSIGNED NOT NULL DEFAULT 0,
    deaths INT UNSIGNED NOT NULL DEFAULT 0,
    contribution INT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY unique_war_participant (war_id, player_guid)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 3. GUILD_WAR_OBJECTIVES — Объекты войны (крепости, ресурсы)
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_war_objectives (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    war_id INT UNSIGNED NOT NULL,
    objective_type VARCHAR(32) NOT NULL, -- 'fortress', 'resource', 'capital'
    objective_id INT UNSIGNED NOT NULL, -- ID крепости/ресурса
    controlling_guild INT UNSIGNED NOT NULL DEFAULT 0, -- 0=никто, 1=атака, 2=оборона
    is_active TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 4. GUILD_WAR_REWARDS — Награды за участие в войне
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_war_rewards (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    war_id INT UNSIGNED NOT NULL,
    guild_id INT UNSIGNED NOT NULL,
    reward_type VARCHAR(32) NOT NULL, -- 'gold', 'spirit', 'item', 'experience'
    reward_amount INT UNSIGNED NOT NULL DEFAULT 0,
    claimed TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 5. GUILD_ALLIANCES — Альянсы между гильдиями
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_alliances (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    guild_a INT UNSIGNED NOT NULL,
    guild_b INT UNSIGNED NOT NULL,
    alliance_type VARCHAR(32) NOT NULL DEFAULT 'neutral', -- 'neutral', 'alliance', 'war'
    created_at INT UNSIGNED NOT NULL,
    expires_at INT UNSIGNED NOT NULL,
    UNIQUE KEY unique_alliance_pair (guild_a, guild_b)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 6. GUILD_WAR_KILLS — Логика kills в войне
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_war_kills (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    war_id INT UNSIGNED NOT NULL,
    killed_player_guid BIGINT UNSIGNED NOT NULL,
    killer_player_guid BIGINT UNSIGNED NOT NULL,
    killed_guild INT UNSIGNED NOT NULL,
    killer_guild INT UNSIGNED NOT NULL,
    kill_time INT UNSIGNED NOT NULL DEFAULT 0,
    is_pvp TINYINT UNSIGNED NOT NULL DEFAULT 1
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 7. GUILD_WAR_SCORE — Счет войны
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS guild_war_score (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    war_id INT UNSIGNED NOT NULL,
    guild_id INT UNSIGNED NOT NULL,
    kills INT UNSIGNED NOT NULL DEFAULT 0,
    deaths INT UNSIGNED NOT NULL DEFAULT 0,
    objectives_captured INT UNSIGNED NOT NULL DEFAULT 0,
    total_damage INT UNSIGNED NOT NULL DEFAULT 0,
    updated_at INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

-- --- Пример данных --- ---

-- Инициализация войн между основными фракциями
INSERT IGNORE INTO guild_wars (guild_a, guild_b, war_state, declared_at, reason, peace_treaty_until) VALUES
(1, 2, 0, UNIX_TIMESTAMP(), 'Возрастание напряжения между Орденом Света и Тёмным Орденом', 0),
(1, 3, 0, UNIX_TIMESTAMP(), 'Конфликт интересов между фракциями', 0),
(2, 3, 0, UNIX_TIMESTAMP(), 'Борьба за ресурсы и территорию', 0);

-- Инициализация盟约 между некоторыми гильдиями
INSERT IGNORE INTO guild_alliances (guild_a, guild_b, alliance_type, created_at, expires_at) VALUES
(1, 3, 'alliance', UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + 86400 * 30), -- Альанс на 30 дней
(2, 3, 'neutral', UNIX_TIMESTAMP(), 0); -- Нейтральные отношения
"