-- ============================================================================
-- Enhancement_System.sql — Система улучшения снаряжения и оружия
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ----------------------------------------------------------------------------
-- 1. ENHANCEMENT_TEMPLATES — Шаблоны улучшения
-- Определяют, какие вещи можно улучшать, с каким шансом успеха и требованиями
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS enhancement_templates (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name_ru VARCHAR(64) NOT NULL,
    name_en VARCHAR(64) NOT NULL,
    description_ru TEXT,
    description_en TEXT,
    item_level_min INT UNSIGNED NOT NULL,
    item_level_max INT UNSIGNED,
    quality_tier TINYINT UNSIGNED NOT NULL DEFAULT 0,
    success_chance_base TINYINT UNSIGNED NOT NULL DEFAULT 80,
    success_chance_extra_per_level TINYINT UNSIGNED NOT NULL DEFAULT 0,
    gold_cost INT UNSIGNED NOT NULL DEFAULT 0,
    required_resource_type VARCHAR(32) NOT NULL DEFAULT 'none',
    required_resource_amount INT UNSIGNED NOT NULL DEFAULT 0,
    soul_bound TINYINT UNSIGNED NOT NULL DEFAULT 0,
    is_degrade_on_fail TINYINT UNSIGNED NOT NULL DEFAULT 1,
    is_stackable TINYINT UNSIGNED NOT NULL DEFAULT 0,
    sort_order INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- Шаблоны улучшения для разных уровней и качеств
INSERT IGNORE INTO enhancement_templates (name_ru, name_en, description_ru, description_en, item_level_min, item_level_max, quality_tier, success_chance_base, success_chance_extra_per_level, gold_cost, required_resource_type, required_resource_amount, soul_bound, is_degrade_on_fail, sort_order) VALUES
-- Новичковые улучшения (уровни 1-10)
('Простое улучшение', 'Basic Enhancement', 'Простое улучшение снаряжения', 'Basic equipment enhancement', 1, 10, 1, 85, 0, 100, 'none', 0, 0, 1, 1),
('Улучшение элемента', 'Elemental Enhancement', 'Улучшение с элементом', 'Adds elemental damage to equipment', 1, 10, 1, 75, 10, 200, 'essence', 1, 0, 1, 2),

-- Средние улучшения (уровни 10-30)
('Укрепление', 'Reinforcement', 'Укрепление снаряжения', 'Strengthens equipment defense', 11, 30, 2, 70, 5, 500, 'shard', 1, 1, 1, 3),
('Магическое усиление', 'Magical Enhancement', 'Магическое улучшение', 'Adds magical properties to equipment', 11, 30, 2, 65, 5, 300, 'essence', 2, 0, 1, 4),

-- Высокие улучшения (уровни 30-50)
('Добытое улучшение', 'Superior Enhancement', 'Добытое улучшение', 'High-quality equipment enhancement', 31, 50, 3, 60, 8, 1000, 'shard', 2, 1, 1, 5),
('Вечное улучшение', 'Eternal Enhancement', 'Вечное улучшение', 'Permanent equipment enhancement', 31, 50, 3, 55, 10, 1500, 'essence', 3, 0, 1, 6),

-- Премиум улучшения (уровни 50-70)
('Многочисленное улучшение', 'Multiple Enhancement', 'Многочисленное улучшение', 'Multiple equipment enhancement', 51, 70, 4, 50, 12, 2000, 'shard', 3, 1, 1, 7),
('Мистическое усиление', 'Mystical Enhancement', 'Мистическое улучшение', 'Mystical equipment properties', 51, 70, 4, 45, 15, 2500, 'essence', 5, 0, 1, 8),

-- Эпичные улучшения (уровни 70-99)
('Грандиозное улучшение', 'Grand Enhancement', 'Грандиозное улучшение', 'Epic equipment enhancement', 71, 99, 5, 40, 20, 5000, 'shard', 5, 1, 1, 9),
('Божественное улучшение', 'Divine Enhancement', 'Божественное улучшение', 'Godlike equipment enhancement', 71, 99, 5, 35, 25, 10000, 'essence', 8, 0, 1, 10);