-- Trade & Vendor schema
CREATE TABLE IF NOT EXISTS `vendor_template` (
    `entry` INT UNSIGNED NOT NULL,
    `item` INT UNSIGNED NOT NULL,
    `maxcount` INT UNSIGNED NOT NULL DEFAULT 0,
    `incrtime` INT UNSIGNED NOT NULL DEFAULT 0,
    `ExtendedCost` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`entry`, `item`)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `trade_log` (
    `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `player_a` BIGINT UNSIGNED NOT NULL,
    `player_b` BIGINT UNSIGNED NOT NULL,
    `item_a` INT UNSIGNED,
    `item_b` INT UNSIGNED,
    `money_a` INT UNSIGNED DEFAULT 0,
    `money_b` INT UNSIGNED DEFAULT 0,
    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB;
