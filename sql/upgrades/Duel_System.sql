-- ============================================================================
-- Duel_System.sql — Система дуэлей 1v1
-- База: dm_world
-- ============================================================================

--
-- ----------------------------------------------------------------------------
-- 1. DUEL_CONFIG — Конфигурация дуэлей
-- ----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS duel_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO duel_config (config_key, config_value, comment) VALUES
('duel_enabled', 'true', 'Включить систему дуэлей'),
('min_duel_level', '10', 'Минимальный уровень для дуэлей'),
('max_duel_level_diff', '20', 'Максимальная разница уровней между участниками'),
('duel_duration_sec', '300', 'Длительность дуэли (SEC)'),
('duel_cooldown_min', '5', 'Кулдаун между дуэлями (мин)'),
('reward_spirit_exp_win', '100', 'Опыт духа за победу'),
('reward_spirit_exp_draw', '50', 'Опыт духа за ничью'),
('entry_fee_gold', '0', 'Вступительный взнос (0=бесплатно)'),
('rating_k_factor', '20', 'Фактор рейтинга K'),
('rating_initial', '1000', 'Начальный рейтинг');
INSERT IGNORE INTO duel_config (config_key, config_value, comment) VALUES
('schedule_day', '6', 'День недели (0=Вс, 6=Сб)'),
('schedule_hour', '18', 'Частота начала'),
('schedule_minute', '0', 'Минута начала');

--
-- ----------------------------------------------------------------------------
-- 2. DUEL_QUEUE — Очередь на дуэль
-- ----------------------------------------------------------------------------
-- DUEL_QUEUE section
