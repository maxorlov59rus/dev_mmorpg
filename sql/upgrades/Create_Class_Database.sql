-- =====================================================================
-- «Чёрная Материя» — классовая БД (dm_characters), TrinityCore-style.
-- Артефакт 2 промпта «Система классов»: структура + данные для 3 классов
-- (Воин / Маг / Разбойник) + частично Жрец/Охотник/Чернокнижник в restrictions.
--
-- ID-контракт (совпадает с PlayerClassMgr.h и Data/ClassInfoDataTable):
--   race:  1 Human, 2 Lumenar, 3 Ashen, 4 Hollowed, 5 Thresholdkin
--   class: 1 Warrior, 2 Priest, 3 Mage, 4 Rogue, 5 Hunter, 6 Warlock
--   faction: 1 Орден Рассвета(Альянс Света), 2 Круг Поглощения(Орден Пепла),
--            3 Гильдия Порога
--   primary_stat: 1 STR 2 AGI 3 STA 4 INT 5 SPI
-- Применение: mysql dm_characters < Create_Class_Database.sql
-- =====================================================================

SET NAMES utf8mb4;

-- ---------------------------------------------------------------------
-- 1) Стартовые статы на уровень (аналог playercreateinfo_item+base stats TRC)
--    Валидируется CHECK'ами: все статы > 0, порядок расовых модификаторов.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS playercreateinfo_classlevelstats (
  race     TINYINT UNSIGNED NOT NULL,
  class    TINYINT UNSIGNED NOT NULL,
  level    TINYINT UNSIGNED NOT NULL,
  strength INT UNSIGNED NOT NULL,
  agility  INT UNSIGNED NOT NULL,
  stamina  INT UNSIGNED NOT NULL,
  intellect INT UNSIGNED NOT NULL,
  spirit   INT UNSIGNED NOT NULL,
  hp_max     INT UNSIGNED NOT NULL,          -- display-ед., Combat переводит в Q16
  power_max  INT UNSIGNED NOT NULL,          -- cap: Rage 100 / Energy 100 / Mana по классу
  armor      INT UNSIGNED NOT NULL,
  spell_power INT UNSIGNED NOT NULL,
  PRIMARY KEY (race, class, level),
  CONSTRAINT fk_pcls_race CHECK (race BETWEEN 1 AND 5),
  CONSTRAINT fk_pcls_class CHECK (class BETWEEN 1 AND 6),
  CONSTRAINT fk_pcls_level CHECK (level BETWEEN 1 AND 60)
) ENGINE=InnoDB;

INSERT INTO playercreateinfo_classlevelstats
  (race, class, level, strength, agility, stamina, intellect, spirit, hp_max, power_max, armor, spell_power) VALUES
-- ВОИН (class 1, Rage)
 (1,1,1, 22,12,20, 6, 8, 120,100, 90, 0),   -- Human×Warrior lvl1
 (3,1,1, 24,11,21, 5, 7, 126,100, 92, 0),   -- Ashen×Warrior lvl1
 (1,1,5, 34,18,31, 9,12, 210,100,140, 0),   -- Human×Warrior lvl5
 (3,1,5, 36,17,33, 8,11, 218,100,144, 0),
-- МАГ (class 3, Mana)
 (1,3,1,  8,14,12,24,18,  78,160, 30,40),
 (2,3,1,  7,15,11,27,20,  82,175, 28,46),   -- Lumenar: +INT/+SPI (расовый бонус магов)
 (2,3,5, 12,21,17,38,28, 132,240, 44,74),
-- РАЗБОЙНИК (class 4, Energy)
 (3,4,1, 12,24,15, 8, 9,  96,100, 55, 0),
 (5,4,1, 11,26,14, 9,10,  94,100, 52, 0),   -- Thresholdkin: +AGI (торговая выучка)
 (3,4,5, 18,38,22,12,14, 158,100, 84, 0),
-- ЖРЕЦ / ОХОТНИК / ЧЕРНОКНИЖНИК —lvl1 заготовки (баланс — дизайн-таблица docs/05)
 (1,2,1,  8,10,13,22,24,  80,170, 32,36),
 (2,2,1,  7, 9,12,25,26,  82,185, 30,42),
 (4,5,1, 14,22,16, 7, 8,  98,100, 50, 0),   -- Hollowed×Hunter
 (3,6,1, 10,13,14,26,12,  88,165, 34,44);   -- Ashen×Warlock (только Круг)

-- ---------------------------------------------------------------------
-- 2) Специализации
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS class_specs (
  spec_id       TINYINT UNSIGNED NOT NULL PRIMARY KEY,
  class_id      TINYINT UNSIGNED NOT NULL,
  name_ru       VARCHAR(64)  NOT NULL,
  name_en       VARCHAR(64)  NOT NULL,
  description_ru TEXT        NOT NULL,
  primary_stat  TINYINT UNSIGNED NOT NULL COMMENT '1=STR 2=AGI 3=STA 4=INT 5=SPI',
  icon_broadcast_id INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'broadcast_text/icon dbc id -> клиентский DataTable',
  KEY idx_spec_class (class_id),
  CONSTRAINT fk_spec_class CHECK (class_id BETWEEN 1 AND 6)
) ENGINE=InnoDB;

INSERT INTO class_specs (spec_id, class_id, name_ru, name_en, description_ru, primary_stat, icon_broadcast_id) VALUES
 (1,1,'Осадный','Siege','Сила разрушения: замедляющие удары по щитам, бонус к Strength-кастам.',1,20001),
 (2,1,'Авангард','Vanguard','Стена щитов: угроза, блок, временные HP от Порчи.',3,20002),
 (3,1,'Дуэлянт','Duelist','Клинок и стойка: крит от Agility, контрудары.',2,20003),
 (4,3,'Мороз','Frost','Ледяные кристаллы Niagara: заморозка, каст сквозь контроль.',4,20011),
 (5,3,'Истечения','Seepage','Школа Чёрной Материи: DoT-вены, рост урона с Corruption.',4,20012),
 (6,3,'Свет','Light','Священные мотыльки: лечение союзников, изгнание Полых.',4,20013),
 (7,4,'Тень','Shadow','Afterimage при спринте: урон из стелса, энергия за убийства.',2,20021),
 (8,4,'Послушник','Altruist','Яды и расходники: отравления, антидоты, торговля на чёрном рынке.',2,20022),
 (9,4,'Порожник','Gatewalker','Портальные кувырки: рывки через Истечения ЧМ.',2,20023);

-- ---------------------------------------------------------------------
-- 3) Автоизучение способностей по уровням (аналог spell_learn_skill TRC)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS class_spells (
  class_id TINYINT UNSIGNED NOT NULL,
  level    TINYINT UNSIGNED NOT NULL,
  spell_id INT UNSIGNED NOT NULL,
  PRIMARY KEY (class_id, level, spell_id),
  KEY idx_cls_spell (spell_id)
) ENGINE=InnoDB;

INSERT INTO class_spells (class_id, level, spell_id) VALUES
 (1,1,100),(1,1,78),      -- Воин: Удар, Уклонение
 (1,4,116),               -- Воин: Рывок
 (3,1,133),(3,1,44008),   -- Маг: Огненная стрела, Полёт камня
 (3,4,122),               -- Маг: Ледяная буря
 (4,1,1752),(4,1,2983),   -- Разбойник: Удар в спину, Скрытность
 (4,4,53),                -- Разбойник: Полоса удушения
 (2,1,2098),              -- Жрец: Малое исцеление
 (5,1,2973),              -- Охотник: Выстрел
 (6,1,686);               -- Чернокнижник: Заклинание крови

-- ---------------------------------------------------------------------
-- 4) Расовые ограничения классов (inverse-список: запрет, а не разрешение)
--    required_faction != 0 => класс привязан к фракции (Warlock — только Круг).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS class_restrictions (
  class_id          TINYINT UNSIGNED NOT NULL,
  forbidden_race_id TINYINT UNSIGNED NOT NULL,
  required_faction  TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '0 = без привязки',
  reason_broadcast_id INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'текст подсказки в UI_CharacterCreate',
  PRIMARY KEY (class_id, forbidden_race_id)
) ENGINE=InnoDB;

INSERT INTO class_restrictions (class_id, forbidden_race_id, required_faction, reason_broadcast_id) VALUES
 (6,1,2,20101),  -- Чернокнижник закрыт Human (и требует фракцию 2 — Круг Поглощения)
 (6,2,2,20101),  -- ...и Lumenar
 (2,3,0,20102),  -- Жрец недоступен пепельным (Орден Пепла отвергает Свет)
 (2,4,0,20102);  -- ...и Полым

-- ---------------------------------------------------------------------
-- 5) Индекс спеки на персонаже + стартовый выбор
-- ---------------------------------------------------------------------
ALTER TABLE characters
  ADD COLUMN spec_id TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `class`,
  ADD COLUMN racial_cooldowns TEXT NULL COMMENT 'JSON {spellId:readyAfterMs} (см. docs/08)';

-- Стартовая экипировка классов (q_1001/q_1002 награды уже ссылаются на эти
-- entry; Bonding=1 BoP, FactionRelated см. item_template в DB_Schema.sql):
INSERT IGNORE INTO items (entry, guid_owner, slot, quantity) VALUES (0,0,0,0); -- no-op guard: schema inventory совместимость

-- ---------------------------------------------------------------------
-- 6) Валидация консистентности (запускается тем же golden-тестом CI):
--    каждая restriction-пара существует; каждая спека принадлежит классу.
-- ---------------------------------------------------------------------
-- SELECT cs.spec_id FROM class_specs cs LEFT JOIN playercreateinfo_classlevelstats p
--   ON p.class=cs.class_id WHERE p.class IS NULL; -- должен быть пуст (для 1,3,4)
