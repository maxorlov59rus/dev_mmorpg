-- ============================================================================
-- 05_quest_def.sql — базовые таблицы квестов, NPC и диалогов (dm_world)
-- Вынесено из sql/upgrades/World_Lore_System.sql: контент в 50-60_Content.sql
-- и других патчах вставляет строки в эти таблицы, поэтому схемы должны
-- существовать до применения upgrades.
-- ============================================================================

CREATE TABLE IF NOT EXISTS city_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    level_range_start TINYINT UNSIGNED NOT NULL,
    level_range_end TINYINT UNSIGNED NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    is_capital TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_unique_city TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS npc_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    title_ru VARCHAR(64) NOT NULL,
    title_en VARCHAR(64) NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    city_id INT UNSIGNED NOT NULL,
    npc_type VARCHAR(32) NOT NULL,
    `rank` VARCHAR(32) NOT NULL DEFAULT 'citizen',
    gender TINYINT UNSIGNED NOT NULL DEFAULT 0,
    race VARCHAR(32) NOT NULL DEFAULT 'human',
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    is_quest_giver TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_trainer TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_special_interact TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS dialogue_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    npc_id INT UNSIGNED NOT NULL,
    dialogue_type VARCHAR(32) NOT NULL,
    text_ru TEXT NOT NULL,
    text_en TEXT NOT NULL,
    emotion VARCHAR(32) NOT NULL DEFAULT 'neutral',
    voice_id INT UNSIGNED,
    priority INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS gossip_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    entry INT UNSIGNED NOT NULL,
    text_ru TEXT NOT NULL,
    text_en TEXT NOT NULL,
    sound_delay INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS quest_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    npc_id INT UNSIGNED NOT NULL DEFAULT 0,
    city_id INT UNSIGNED,
    faction_id INT UNSIGNED,
    title_ru VARCHAR(64) NOT NULL,
    title_en VARCHAR(64) NOT NULL,
    description_ru TEXT NOT NULL,
    description_en TEXT NOT NULL,
    level_min TINYINT UNSIGNED NOT NULL,
    level_max TINYINT UNSIGNED NOT NULL,
    quest_type VARCHAR(32) NOT NULL,
    quest_category VARCHAR(32) NOT NULL,
    objectives TEXT NOT NULL,
    reward_type VARCHAR(32) NOT NULL DEFAULT 'gold',
    reward_value VARCHAR(128) NOT NULL DEFAULT '0',
    reward_amount INT UNSIGNED NOT NULL DEFAULT 0,
    time_limit INT UNSIGNED DEFAULT 0,
    is_repeatable TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS monster_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    level TINYINT UNSIGNED NOT NULL,
    faction_id INT UNSIGNED NOT NULL,
    hp INT UNSIGNED NOT NULL,
    mana INT UNSIGNED NOT NULL DEFAULT 0,
    exp_reward INT UNSIGNED NOT NULL DEFAULT 0,
    gold_reward INT UNSIGNED NOT NULL DEFAULT 0,
    attack_power INT UNSIGNED NOT NULL DEFAULT 0,
    defense INT UNSIGNED NOT NULL DEFAULT 0,
    quest_id INT UNSIGNED DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS loot_table (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    item_id INT UNSIGNED NOT NULL,
    monster_id INT UNSIGNED NOT NULL,
    chance TINYINT UNSIGNED NOT NULL DEFAULT 0,
    quantity INT UNSIGNED NOT NULL DEFAULT 1,
    description_ru TEXT,
    description_en TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS resource_table (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    type VARCHAR(32) NOT NULL,
    stack_size INT UNSIGNED NOT NULL DEFAULT 1,
    respawn_time INT UNSIGNED NOT NULL DEFAULT 0,
    gathering_level_min TINYINT UNSIGNED NOT NULL DEFAULT 1,
    description_ru TEXT,
    description_en TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS creature_template (
    entry              MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
    name               VARCHAR(64) NOT NULL,
    subname            VARCHAR(64) NULL,
    level              TINYINT UNSIGNED NOT NULL DEFAULT 1,
    type               TINYINT UNSIGNED NOT NULL DEFAULT 0,
    faction_A          SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    faction_H          SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    faction_alliance   SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    faction_horde      SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    min_level          TINYINT UNSIGNED NOT NULL DEFAULT 1,
    max_level          TINYINT UNSIGNED NOT NULL DEFAULT 1,
    minlevel           TINYINT UNSIGNED NOT NULL DEFAULT 1,
    maxlevel           TINYINT UNSIGNED NOT NULL DEFAULT 1,
    dmg_modifier       DECIMAL(6,3) NOT NULL DEFAULT 1.000,
    health_modifier    DECIMAL(6,3) NOT NULL DEFAULT 1.000,
    damage_mult        DECIMAL(6,3) NOT NULL DEFAULT 1.000,
    health_mult        DECIMAL(6,3) NOT NULL DEFAULT 1.000,
    lootid             MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
    pickpocketloot     MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
    mingold            INT UNSIGNED NOT NULL DEFAULT 0,
    maxgold            INT UNSIGNED NOT NULL DEFAULT 0,
    unit_flags         INT UNSIGNED NOT NULL DEFAULT 0,
    unit_flags2        INT UNSIGNED NOT NULL DEFAULT 0,
    type_flags         INT UNSIGNED NOT NULL DEFAULT 0,
    mechanic_immune_mask INT UNSIGNED NOT NULL DEFAULT 0,
    script_name        VARCHAR(64) NOT NULL DEFAULT '',
    KEY idx_factions (faction_A, faction_H)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS creature_spawn (
    Guid        INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    Entry       MEDIUMINT UNSIGNED NOT NULL,
    MapId       SMALLINT UNSIGNED NOT NULL,
    X           FLOAT NOT NULL,
    Y           FLOAT NOT NULL,
    Z           FLOAT NOT NULL,
    Orientation FLOAT NOT NULL DEFAULT 0,
    SpawnsMask  INT UNSIGNED NOT NULL DEFAULT 1,
    RespawnTime INT UNSIGNED NOT NULL DEFAULT 300,
    KEY idx_entry (Entry),
    KEY idx_map (MapId)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS creature_loot_template (
    Entry   MEDIUMINT UNSIGNED NOT NULL,
    Item    MEDIUMINT UNSIGNED NOT NULL,
    Chance  FLOAT NOT NULL DEFAULT 0,
    Comment VARCHAR(255) NULL,
    PRIMARY KEY (Entry, Item)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS quest_dialogue (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    quest_id INT UNSIGNED NOT NULL,
    npc_entry INT UNSIGNED NOT NULL,
    dialogue_type TINYINT UNSIGNED NOT NULL,
    locale VARCHAR(5) NOT NULL DEFAULT 'ru',
    text TEXT NOT NULL,
    INDEX idx_quest (quest_id),
    INDEX idx_npc (npc_entry)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS campaign_def (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(96) NOT NULL,
    description TEXT NULL,
    faction_id TINYINT UNSIGNED NOT NULL DEFAULT 1,
    level_range_start TINYINT UNSIGNED NOT NULL DEFAULT 1,
    level_range_end TINYINT UNSIGNED NOT NULL DEFAULT 10,
    INDEX idx_faction_level (faction_id, level_range_start)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS item_template (
  entry        MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  class        TINYINT UNSIGNED NOT NULL,
  subclass     TINYINT UNSIGNED NOT NULL,
  name         VARCHAR(64) NOT NULL,
  displayid    INT UNSIGNED NOT NULL DEFAULT 0,
  Quality      TINYINT UNSIGNED NOT NULL DEFAULT 1,
  ItemLevel    SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  RequiredLevel TINYINT UNSIGNED NOT NULL DEFAULT 1,
  AllowableClass INT NOT NULL DEFAULT -1,
  AllowableRace  INT NOT NULL DEFAULT -1,
  Bonding      TINYINT UNSIGNED NOT NULL DEFAULT 0,
  FactionRelated TINYINT UNSIGNED NOT NULL DEFAULT 0,
  FactionReqRank TINYINT UNSIGNED NOT NULL DEFAULT 0,
  BuyCount     TINYINT UNSIGNED NOT NULL DEFAULT 1,
  BuyPrice     INT UNSIGNED NOT NULL DEFAULT 0,
  SellPrice    INT UNSIGNED NOT NULL DEFAULT 0,
  Material     TINYINT NOT NULL DEFAULT 0,
  KEY idx_faction (FactionRelated, FactionReqRank)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
CREATE TABLE IF NOT EXISTS gameobject_template (
    entry          MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
    type           TINYINT UNSIGNED NOT NULL DEFAULT 0,
    name           VARCHAR(96) NOT NULL,
    displayid      MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
    required_skill SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    lootid         MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
    zone           SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    spawn_note     VARCHAR(255) NULL,
    INDEX idx_zone (zone)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS gameobject_loot_template (
    Entry   MEDIUMINT UNSIGNED NOT NULL,
    Item    MEDIUMINT UNSIGNED NOT NULL,
    Chance  FLOAT NOT NULL DEFAULT 100.0,
    Comment VARCHAR(128) NULL,
    PRIMARY KEY (Entry, Item)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;