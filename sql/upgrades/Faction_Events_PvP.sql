-- Faction Relations & Events & PvP Events
CREATE TABLE IF NOT EXISTS `faction_relations` (
    `faction_a` INT UNSIGNED NOT NULL,
    `faction_b` INT UNSIGNED NOT NULL,
    `relation` TINYINT NOT NULL DEFAULT 0, -- -2 hostile, -1 unfriendly, 0 neutral, 1 friendly, 2 allied
    PRIMARY KEY (`faction_a`, `faction_b`)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `world_events` (
    `event_id` INT UNSIGNED NOT NULL,
    `name` VARCHAR(64) NOT NULL,
    `start_time` DATETIME NOT NULL,
    `end_time` DATETIME NOT NULL,
    `active` TINYINT NOT NULL DEFAULT 0,
    PRIMARY KEY (`event_id`)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `pvp_events` (
    `event_id` INT UNSIGNED NOT NULL,
    `type` TINYINT NOT NULL, -- 1 battleground, 2 arena, 3 open-world
    `min_level` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `max_level` TINYINT UNSIGNED NOT NULL DEFAULT 95,
    `reward_honor` INT UNSIGNED NOT NULL DEFAULT 0,
    `reward_item` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`event_id`)
) ENGINE=InnoDB;
