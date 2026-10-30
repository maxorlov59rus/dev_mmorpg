-- Achievement Shop table
CREATE TABLE IF NOT EXISTS achievement_shop (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    cost_achievement_points INT UNSIGNED NOT NULL DEFAULT 0,
    item_entry INT UNSIGNED NOT NULL,
    category VARCHAR(32) NOT NULL, -- 'buff', 'consumable', 'cosmetic', 'currency'
    duration_minutes INT UNSIGNED NOT NULL DEFAULT 0,
    is_limited TINYINT UNSIGNED NOT NULL DEFAULT 0,
    daily_limit INT UNSIGNED NOT NULL DEFAULT 1,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Insert shop items
INSERT IGNORE INTO achievement_shop (name_ru, name_en, cost_achievement_points, item_entry, category, duration_minutes, is_limited, daily_limit, sort_order) VALUES
-- Stat buffs
('Купол stärовка', 'Stat Boost', 100, 10101, 'buff', 60, 0, 3, 1),
('Ускорение опыта', 'Experience Boost', 150, 10102, 'buff', 120, 0, 2, 2),
('Бонус к золоту', 'Gold Bonus', 75, 10103, 'buff', 30, 0, 5, 3),

-- Cosmetic
('Золотой冠', 'Gold Crown', 200, 20001, 'cosmetic', 0, 0, 1, 4),
('Меч почетного чемпиона', 'Champion Sword', 300, 30001, 'cosmetic', 0, 0, 1, 5),

-- Consumables
('Зелье мудрости', 'Wisdom Potion', 50, 40001, 'consumable', 30, 1, 1, 6),
('Зелье ловкости', 'Dexterity Potion', 50, 40002, 'consumable', 30, 1, 1, 7);