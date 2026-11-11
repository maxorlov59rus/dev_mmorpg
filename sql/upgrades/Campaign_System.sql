-- Campaign_System.sql — Система кампаний с фракциями
-- База: dm_world
-- Добавляет колонку faction_id в таблицу campaign_def

USE dm_world;

-- ----------------------------------------------------------------------------
-- 1. Добавляем колонку faction_id в таблицу campaign_def
-- ----------------------------------------------------------------------------
SET @c := (SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='dm_world' AND TABLE_NAME='campaign_def' AND COLUMN_NAME='faction_id');
SET @q := IF(@c=0, 'ALTER TABLE campaign_def ADD COLUMN faction_id INT UNSIGNED NOT NULL DEFAULT 1 COMMENT ''1=Light, 2=Dark, 3=Neutral'' AFTER level_range_end', 'DO 0');
PREPARE s FROM @q; EXECUTE s; DEALLOCATE PREPARE s;

-- ----------------------------------------------------------------------------
-- 2. Обновляем существующие кампании набора данных
-- ----------------------------------------------------------------------------
UPDATE campaign_def SET faction_id = 1 WHERE id IN (1, 2, 3); -- Света
UPDATE campaign_def SET faction_id = 2 WHERE id IN (5, 6, 7); -- Тьмы
UPDATE campaign_def SET faction_id = 3 WHERE id IN (9, 10); -- Нейтральные

-- ----------------------------------------------------------------------------
-- 3. Добавляем ограничение уникальности по фракции и уровню
-- ----------------------------------------------------------------------------
CREATE UNIQUE INDEX idx_campaign_faction_level ON campaign_def (faction_id, level_range_start);