-- ============================================================================
-- IntegrityVerification.sql -- Система проверки целостности сервера
-- База: dm_world
-- ============================================================================

USE dm_world;

-- ============================================================================
-- 1. FILE_INTEGRITY -- Таблица хешей файлов сервера
-- ============================================================================
CREATE TABLE IF NOT EXISTS file_integrity (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    file_path VARCHAR(255) NOT NULL,
    file_checksum VARCHAR(32) NOT NULL,
    file_size BIGINT UNSIGNED NOT NULL,
    last_checked TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    is_valid TINYINT UNSIGNED NOT NULL DEFAULT 1,
    UNIQUE KEY unique_file_path (file_path)
) ENGINE=InnoDB;

-- ============================================================================
-- 2. SERVER_INTEGRITY_LOG -- Лог проверок целостности
-- ============================================================================
CREATE TABLE IF NOT EXISTS server_integrity_log (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    check_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    checked_by VARCHAR(100),
    total_files INT UNSIGNED,
    valid_files INT UNSIGNED,
    invalid_files INT UNSIGNED,
    overall_status TINYINT UNSIGNED NOT NULL DEFAULT 1,
    notes TEXT
) ENGINE=InnoDB;

-- ============================================================================
-- 3. CONFIG_INTEGRITY -- Инtegrity check конфигурации
-- ============================================================================
CREATE TABLE IF NOT EXISTS config_integrity (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    config_key VARCHAR(100) NOT NULL,
    expected_value VARCHAR(255) NOT NULL,
    last_verified TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_valid TINYINT UNSIGNED NOT NULL DEFAULT 1,
    UNIQUE KEY unique_config_key (config_key)
) ENGINE=InnoDB;

-- ============================================================================
-- 4. INITIAL_INTEGRITY_CHECK -- Результат первой проверки при установке
-- ============================================================================
CREATE TABLE IF NOT EXISTS initial_integrity_check (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    install_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    total_files INT UNSIGNED,
    valid_files INT UNSIGNED,
    invalid_files INT UNSIGNED,
    overall_status TINYINT UNSIGNED NOT NULL DEFAULT 1,
    admin_user VARCHAR(100),
    notes TEXT
) ENGINE=InnoDB;

-- ============================================================================
-- 5. SERVER_VERSION -- Версия сервера для отслеживания изменений
-- ============================================================================
CREATE TABLE IF NOT EXISTS server_version (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    version_number VARCHAR(50) NOT NULL,
    release_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    notes TEXT,
    is_current TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- ============================================================================
-- 6. PACKET_INTEGRITY -- Интеграность пакетов сети
-- ============================================================================
CREATE TABLE IF NOT EXISTS packet_integrity (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    packet_id INT UNSIGNED NOT NULL,
    packet_name VARCHAR(100) NOT NULL,
    expected_checksum VARCHAR(32) NOT NULL,
    last_verified TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_valid TINYINT UNSIGNED NOT NULL DEFAULT 1,
    violation_count INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- ============================================================================
-- ИНСЕРТИРОВАТЬ данные проверки целостности
-- ============================================================================
INSERT IGNORE INTO file_integrity (file_path, file_checksum, file_size, is_valid) VALUES
('/dm_server/src/game/ServerMgr.h', 'placeholder_checksum_1', 1024, 1),
('/dm_server/src/game/ServerMgr.cpp', 'placeholder_checksum_2', 2048, 1),
('/dm_server/src/game/Network/Handlers/PacketHandlers.cpp', 'placeholder_checksum_3', 3072, 1),
('/dm_server/src/game/Network/Handlers/PacketHandlers.h', 'placeholder_checksum_4', 512, 1),
('/dm_server/src/game/Config/Config.h', 'placeholder_checksum_5', 256, 1),
('/dm_server/src/game/Config/Config.cpp', 'placeholder_checksum_6', 1024, 1),
('/dm_server/src/game/Campaigns/CampaignMgr.h', 'placeholder_checksum_7', 512, 1),
('/dm_server/src/game/Campaigns/CampaignMgr.cpp', 'placeholder_checksum_8', 768, 1),
('/dm_server/src/game/Integrity/ChecksumCalculator.h', 'placeholder_checksum_9', 256, 1),
('/dm_server/src/game/Integrity/ChecksumCalculator.cpp', 'placeholder_checksum_10', 512, 1);

-- Инициализация записи проверки целостности
INSERT IGNORE INTO server_integrity_log (checked_by, total_files, valid_files, invalid_files, overall_status) VALUES
('SystemInit', 10, 10, 0, 1);

-- Инициализация версии сервера
INSERT IGNORE INTO server_version (version_number, notes, is_current) VALUES
('1.0.0', 'Initial release with core systems', 1);

-- Инициализация проверки целостности при установке
INSERT IGNORE INTO initial_integrity_check (total_files, valid_files, invalid_files, overall_status, admin_user) VALUES
(10, 10, 0, 1, 'SystemAdmin');