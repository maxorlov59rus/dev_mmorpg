-- ============================================================================
-- Google_Auth.sql — Авторизация через Google
-- База: dm_auth + dm_characters
-- ============================================================================

USE dm_auth;

-- ---------------------------------------------------------------------------
-- 1. GOOGLE_AUTH_CONFIG — Конфигурация Google OAuth
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS google_auth_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(512) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO google_auth_config (config_key, config_value, comment) VALUES
('google_client_id', 'YOUR_GOOGLE_CLIENT_ID.apps.googleusercontent.com', 'Google OAuth Client ID'),
('google_client_secret', 'YOUR_GOOGLE_CLIENT_SECRET', 'Google OAuth Client Secret'),
('google_redirect_uri', 'http://localhost:8080/auth/google/callback', 'Redirect URI'),
('google_auth_enabled', 'true', 'Включить Google авторизацию'),
('google_scope', 'openid email profile', 'OAuth scope'),
('token_expiry_hours', '24', 'Время жизни токена (часы)'),
('refresh_token_expiry_days', '30', 'Время жизни refresh токена (дни)');

-- ---------------------------------------------------------------------------
-- 2. GOOGLE_AUTH_TOKENS — Токены авторизации
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS google_auth_tokens (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    account_id BIGINT UNSIGNED NOT NULL,
    google_id VARCHAR(128) NOT NULL,
    access_token VARCHAR(512) NOT NULL,
    refresh_token VARCHAR(512),
    id_token VARCHAR(1024),
    token_type VARCHAR(32) NOT NULL DEFAULT 'Bearer',
    expires_at INT UNSIGNED NOT NULL,
    created_at INT UNSIGNED NOT NULL,
    updated_at INT UNSIGNED NOT NULL,
    INDEX idx_account (account_id),
    INDEX idx_google_id (google_id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 3. GOOGLE_AUTH_ACCOUNTS — Связь Google аккаунтов с игровыми
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS google_auth_accounts (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    google_id VARCHAR(128) NOT NULL UNIQUE,
    email VARCHAR(256) NOT NULL,
    name VARCHAR(128),
    picture_url VARCHAR(512),
    locale VARCHAR(16),
    account_id BIGINT UNSIGNED,
    created_at INT UNSIGNED NOT NULL,
    last_login_at INT UNSIGNED NOT NULL,
    INDEX idx_google_id (google_id),
    INDEX idx_email (email),
    INDEX idx_account (account_id)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------------
-- 4. LOADING_SCREEN_CONFIG — Конфигурация загрузочного экрана
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS loading_screen_config (
    config_key VARCHAR(64) NOT NULL PRIMARY KEY,
    config_value VARCHAR(255) NOT NULL,
    comment VARCHAR(255)
) ENGINE=InnoDB;

INSERT IGNORE INTO loading_screen_config (config_key, config_value, comment) VALUES
('loading_screen_enabled', 'true', 'Включить загрузочный экран'),
('show_google_auth', 'true', 'Показать кнопку Google авторизации'),
('show_logo', 'true', 'Показать логотип'),
('show_version', 'true', 'Показать версию'),
('background_image', 'UI/Loading/background', 'Фоновое изображение'),
('logo_image', 'UI/Loading/logo', 'Логотип'),
('animation_type', 'fade', 'Тип анимации (fade/slide)'),
('min_display_time_ms', '2000', 'Минимальное время показа (мс)'),
('google_auth_button_text_ru', 'Войти через Google', 'Текст кнопки (RU)'),
('google_auth_button_text_en', 'Sign in with Google', 'Текст кнопки (EN)');

USE dm_characters;

-- ---------------------------------------------------------------------------
-- 5. CHARACTER_GOOGLE_LINK — Связь персонажей с Google аккаунтами
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS character_google_link (
    character_guid BIGINT UNSIGNED NOT NULL PRIMARY KEY,
    google_account_id BIGINT UNSIGNED NOT NULL,
    linked_at INT UNSIGNED NOT NULL,
    INDEX idx_google_account (google_account_id)
) ENGINE=InnoDB;