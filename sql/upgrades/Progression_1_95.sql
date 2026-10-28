-- =====================================================================
-- «Чёрная Материя» — Прогрессия 1–95: навыки, таланты, экипировка.
-- Артефакты 2–4 промпта «Система навыков/талантов/экипировки».
-- Контракт ID: PlayerClassMgr.h (class 1 Warrior, 3 Mage, 4 Rogue),
-- ProgressionCore.h (уровни, тиры), docs/09-Прогрессия-1-95.md.
-- Применение: mysql dm_characters < Progression_1_95.sql; mysql dm_world < (часть loot)
-- =====================================================================

SET NAMES utf8mb4;

-- ---------------------------------------------------------------------
-- 1) class_spells уже существует (Create_Class_Database.sql). Расширяем
--    расписание способностей на 1..95 по частотным поясам docs/09 §2:
--    L1-10 каждые 2 ур.; L11-40 каждые 3; L41-65 каждые 5; L66-90 ~каждые 6;
--    L91-95 prestige-апгрейды (каждый уровень).
--    SpellID диапазон: Warrior 10xxx, Mage 30xxx, Rogue 40xxx.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS class_spells_full (
  class     TINYINT UNSIGNED NOT NULL,
  level     TINYINT UNSIGNED NOT NULL,
  spell_id  MEDIUMINT UNSIGNED NOT NULL,
  spell_name VARCHAR(64) NOT NULL,
  cadence   ENUM('core','rotation','cooldown','burst','prestige') NOT NULL,
  replaces  MEDIUMINT UNSIGNED NOT NULL DEFAULT 0, -- prestige: улучшаемый спелл
  PRIMARY KEY (class, spell_id),
  KEY idx_cls_lvl (class, level),
  CHECK (level BETWEEN 1 AND 95)
) ENGINE=InnoDB;

-- ===== ВОИН (class 1) =====
INSERT INTO class_spells_full (class, level, spell_id, spell_name, cadence, replaces) VALUES
-- core L1-10
(1,1,10001,'Удар мечом',            'core',0),
(1,3,10002,'Рывок',                 'core',0),
(1,5,10003,'Оглушительный удар',    'core',0),
(1,7,10004,'Шаросек',               'core',0),
(1,9,10005,'Боевой клич',           'core',0),
(1,10,10006,'Стойка обороны',       'core',0), -- спека-гейт L10
-- rotation L11-40 (каждые 3)
(1,11,10011,'Молот ярости',         'rotation',0),
(1,14,10014,'Круговой рубящий',     'rotation',0),
(1,17,10017,'Прыжок героя',         'rotation',0),
(1,20,10020,'Разрывной удар',       'rotation',0),
(1,23,10023,'Стена щитов',          'rotation',0),
(1,26,10026,'Кровавая жажда',       'rotation',0),
(1,29,10029,'Таран',                'rotation',0),
(1,32,10032,'Вихрь клинков',        'rotation',0),
(1,35,10035,'Несокрушимость',       'rotation',0),
(1,38,10038,'Громовой разлом',      'rotation',0),
-- cooldown/utility L41-65 (каждые 5)
(1,41,10041,'Ярость берсерка',      'cooldown',0),
(1,46,10046,'Неудержимый рев',      'cooldown',0),
(1,51,10051,'Щит отражения',        'cooldown',0),
(1,56,10056,'Землетрясение',        'cooldown',0),
(1,61,10061,'Авангардный рывок',    'cooldown',0),
(1,65,10065,'Бастион',              'cooldown',0),
-- burst L66-90 («эйсы в рукаве»)
(1,66,10066,'Казнь',                'burst',0),
(1,72,10072,'Легион щитов',         'burst',0),
(1,78,10078,'Сердце войны',         'burst',0),
(1,84,10084,'Обвал неба',           'burst',0),
(1,90,10090,'Бессмертный авангард', 'burst',0),
-- prestige L91-95 (апгрейды существующих)
(1,91,10091,'Казнь: второй заряд',        'prestige',10066),
(1,92,10092,'Ярость берсерка: -10% КД',   'prestige',10041),
(1,93,10093,'Вихрь клинков: +1 цель',     'prestige',10032),
(1,94,10094,'Бастион: чистка замедлений', 'prestige',10065),
(1,95,10095,'Аватар Чёрной Материи',      'prestige',10090);

-- ===== МАГ (class 3) =====
INSERT INTO class_spells_full (class, level, spell_id, spell_name, cadence, replaces) VALUES
(3,1,30001,'Огненный шар',        'core',0),
(3,3,30003,'Ледяная стрела',      'core',0),
(3,5,30005,'Мерцание',            'core',0),
(3,7,30007,'Ледяные оковы',       'core',0),
(3,9,30009,'Барьер льда',         'core',0),
(3,10,30010,'Огненный столб',     'core',0), -- спец: Огонь
(3,11,30011,'Метель',             'rotation',0),
(3,14,30014,'Огненная стена',      'rotation',0),
(3,17,30017,'Цепная молния',       'rotation',0),
(3,20,30020,'Полиморф',            'rotation',0),
(3,23,30023,'Ледяная буря',        'rotation',0),
(3,26,30026,'Жар взрыва',          'rotation',0),
(3,29,30029,'Зеркальные иллюзии',  'rotation',0),
(3,32,30032,'Кольцо огня',         'rotation',0),
(3,35,30035,'Низвержение',         'rotation',0),
(3,38,30038,'Хрустальный шип',     'rotation',0),
(3,41,30041,'Прилив маны',         'cooldown',0),
(3,45,30045,'Ледяная глыба',       'cooldown',0),
(3,46,30046,'Алтарь пламени',      'cooldown',0),
(3,51,30051,'Ледяное прикосновение','cooldown',0),
(3,56,30056,'Искажение времени',   'cooldown',0),
(3,61,30061,'Метеор',              'cooldown',0),
(3,65,30065,'Криосфера',           'cooldown',0),
(3,66,30066,'Феникс',              'burst',0),
(3,72,30072,'Ледниковый период',   'burst',0),
(3,78,30078,'Северное сияние',     'burst',0),
(3,84,30084,'Падение кометы',      'burst',0),
(3,90,30090,'Массовая телепортация','burst',0),
(3,91,30091,'Метеор: двойной зальп','prestige',30061),
(3,92,30092,'Мерцание: 3 заряда',  'prestige',30005),
(3,93,30093,'Огненный шар: взрыв', 'prestige',30001),
(3,94,30094,'Прилив маны: -15% КД','prestige',30041),
(3,95,30095,'Повелитель Истечения', 'prestige',30090);

-- ===== РАЗБОЙНИК (class 4) =====
INSERT INTO class_spells_full (class, level, spell_id, spell_name, cadence, replaces) VALUES
(4,1,40001,'Удар кинжалом',       'core',0),
(4,3,40003,'Подкрадывание',       'core',0),
(4,5,40005,'Отравленный клинок',  'core',0),
(4,7,40007,'Рывок теней',         'core',0),
(4,9,40009,'Ослепляющий бросок',  'core',0),
(4,10,40010,'Невидимость',        'core',0),
(4,11,40011,'Веер ножей',         'rotation',0),
(4,14,40014,'Удар в сердце',      'rotation',0),
(4,17,40017,'Танец кинжалов',     'rotation',0),
(4,20,40020,'Яд скорпиона',       'rotation',0),
(4,23,40023,'Прыжок со спины',    'rotation',0),
(4,26,40026,'Дымовая завеса',     'rotation',0),
(4,29,40029,'Касание вампира',    'rotation',0),
(4,32,40032,'Метка убийцы',       'rotation',0),
(4,35,40035,'Теневой шаг+',       'rotation',0),
(4,38,40038,'Клинок хаоса',       'rotation',0),
(4,41,40041,'Внутренний покой',   'cooldown',0),
(4,46,40046,'Отрава рун',         'cooldown',0),
(4,51,40051,'Пороговый кувырок',  'cooldown',0),
(4,56,40056,'Засада',             'cooldown',0),
(4,61,40061,'Тысяча cuts',        'cooldown',0),
(4,65,40065,'Марка смерти',       'cooldown',0),
(4,66,40066,'Казнь из тени',      'burst',0),
(4,72,40072,'Хоровод клинков',    'burst',0),
(4,78,40078,'Призрак Порога',     'burst',0),
(4,84,40084,'Убийство вслепую',   'burst',0),
(4,90,40090,'Танец тысячи лезвий','burst',0),
(4,91,40091,'Невидимость: +2с',   'prestige',40010),
(4,92,40092,'Рывок теней: 3 заряда','prestige',40007),
(4,93,40093,'Яд: наложение стека','prestige',40005),
(4,94,40094,'Марка смерти: -10% КД','prestige',40065),
(4,95,40095,'Лик Безмолвия',      'prestige',40090);

-- ---------------------------------------------------------------------
-- 2) ДРЕВО ТАЛАНТОВ (docs/09 §3): 3 ветки x 7 рядов x до 4 узлов.
--    Очки: 1/уровень с L10 => 86 очков (TOTAL_TALENT_POINTS).
--    Row7 = Prestige (стоимость 3 очка за ранг, требует L91+ и 27+ в ветке).
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS talent_trees (
  tree_id   SMALLINT UNSIGNED NOT NULL,
  class     TINYINT UNSIGNED NOT NULL,
  name      VARCHAR(64) NOT NULL,
  spec_id   TINYINT UNSIGNED NOT NULL DEFAULT 0, -- привязка к SpecId (0 = общая)
  PRIMARY KEY (tree_id)
) ENGINE=InnoDB;

INSERT INTO talent_trees (tree_id, class, name, spec_id) VALUES
(101,1,'Оружие',1),(102,1,'Неистовство',2),(103,1,'Защита',3),   -- Воин
(301,3,'Огонь',4),(302,3,'Мороз',4),(303,3,'Истечение',5),       -- Маг
(401,4,'Тень',7),(402,4,'Послушник',8),(403,4,'Порожник',9);     -- Разбойник

CREATE TABLE IF NOT EXISTS talents (
  talent_id    MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
  tree_id      SMALLINT UNSIGNED NOT NULL,
  row_idx      TINYINT UNSIGNED NOT NULL,      -- 1..7
  col_idx      TINYINT UNSIGNED NOT NULL,      -- 1..4
  max_rank     TINYINT UNSIGNED NOT NULL DEFAULT 1,
  type         ENUM('passive','active','mechanic') NOT NULL DEFAULT 'passive',
  req_points   TINYINT UNSIGNED NOT NULL DEFAULT 0, -- очки в ветке для доступа
  req_level    TINYINT UNSIGNED NOT NULL DEFAULT 10,
  aura_spell   MEDIUMINT UNSIGNED NOT NULL DEFAULT 0, -- пассивка = аура
  active_spell MEDIUMINT UNSIGNED NOT NULL DEFAULT 0, -- активный талант = кнопка
  mod_json     TEXT NULL,                        -- {"stat":"CRIT_DMG","value":10}
  prestige     TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- 1 = Row7 (cost 3)
  vfx_asset    VARCHAR(128) NULL,                -- путь Niagara для клиента
  PRIMARY KEY (talent_id),
  UNIQUE KEY uq_node (tree_id, row_idx, col_idx),
  CHECK (row_idx BETWEEN 1 AND 7),
  CHECK (col_idx BETWEEN 1 AND 4),
  CHECK (req_level BETWEEN 10 AND 95)
) ENGINE=InnoDB;

-- Пример ветки «Оружие» воина (101): компактный набор для контракта UI
INSERT INTO talents (tree_id,row_idx,col_idx,max_rank,type,req_points,aura_spell,mod_json,prestige,vfx_asset) VALUES
(101,1,1,3,'passive',0,90101,'{"stat":"MELEE_HASTE_PCT","value":3}',0,NULL),
(101,1,2,3,'passive',0,90102,'{"stat":"STR_PCT","value":3}',0,NULL),
(101,2,1,2,'passive',4,90103,'{"stat":"CRIT_DMG_PCT","value":8}',0,NULL),
(101,2,2,2,'mechanic',4,90104,'{"effect":"WHIRLPLUS_TARGET","value":1}',0,NULL),
(101,3,1,1,'active',10,0,'{"active_spell":90110}',0,'/Game/VFX/Talents/NS_WeaponRoar'),
(101,4,1,3,'passive',14,90111,'{"stat":"DAMAGE_DONE_PCT","value":4}',0,NULL),
(101,5,1,2,'passive',19,90112,'{"stat":"RAGE_GEN_PCT","value":10}',0,NULL),
(101,6,1,1,'mechanic',23,90113,'{"effect":"EXECUTE_LOW_HP","value":20}',0,NULL),
(101,7,1,1,'active',27,0,'{"active_spell":90114,"cost_points":3}',1,'/Game/VFX/Talents/NS_BladeAvatar');

-- Зеркальные наборы для 102/103/3xx/4xx генерируются по тому же шаблону
-- (полная матрица 3x7x4 — задача контент-пайплайна, см. docs/09 §3).

-- ---------------------------------------------------------------------
-- 3) ЭКИПИРОВКА: ItemTierMatrix (docs/09 §4). Наследуем item_template из
--    DB_Schema.sql; добавляем справочник тиров + бюджетную таблицу.
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS item_tier_matrix (
  tier        TINYINT UNSIGNED NOT NULL,      -- ItemTier enum ProgressionCore.h
  name        VARCHAR(32) NOT NULL,
  min_ilvl    SMALLINT UNSIGNED NOT NULL,
  max_ilvl    SMALLINT UNSIGNED NOT NULL,
  armor_mult  DECIMAL(4,2) NOT NULL DEFAULT 1.20, -- Armor = ilvl * mult
  dps_mult    DECIMAL(4,2) NOT NULL DEFAULT 0.80, -- WeaponDPS = ilvl * mult
  has_sockets TINYINT UNSIGNED NOT NULL DEFAULT 0,
  has_proc    TINYINT UNSIGNED NOT NULL DEFAULT 0,
  secondary_stats ENUM('none','partial','full') NOT NULL DEFAULT 'none',
  PRIMARY KEY (tier)
) ENGINE=InnoDB;

INSERT INTO item_tier_matrix VALUES
(0,'Common (серое/белое)',1 ,19 ,1.20,0.80,0,0,'none'),
(1,'Uncommon (зелёное, Dungeon Greens)',20,49,1.20,0.80,0,0,'none'),
(2,'Rare (синее, стихийный урон)',      50,59,1.20,0.80,1,0,'partial'),
(3,'Epic (Raid Epics, вторички)',        60,79,1.20,0.80,1,1,'full'),
(4,'Mythic (Endgame 80–95, проки+сокеты)',80,95,1.20,0.80,2,1,'full');

-- Armor Matrix классов: какие подтипы брони доступны на каких уровнях
-- (Воин: Mail до 40, далее Plate; Маг: Cloth; Разбойник: Leather)
CREATE TABLE IF NOT EXISTS class_armor_matrix (
  class        TINYINT UNSIGNED NOT NULL,
  armor_type   TINYINT UNSIGNED NOT NULL, -- ArmorSubtype: 1Cloth 2Leather 3Mail 4Plate
  min_level    TINYINT UNSIGNED NOT NULL,
  max_level    TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (class, armor_type, min_level)
) ENGINE=InnoDB;

INSERT INTO class_armor_matrix VALUES
(1,3,1,40),(1,4,40,95),      -- Воин: Mail 1–40, Plate 40–95
(3,1,1,95),                  -- Маг: Cloth всегда
(4,2,1,95);                  -- Разбойник: Leather всегда

-- Стартовые предметы специализаций (награда q_1104) — связка с LFG-ролью
CREATE TABLE IF NOT EXISTS spec_start_items (
  class     TINYINT UNSIGNED NOT NULL,
  spec_id   TINYINT UNSIGNED NOT NULL,
  item_id   MEDIUMINT UNSIGNED NOT NULL,
  role_mask TINYINT UNSIGNED NOT NULL, -- 1=TANK 2=HEALER 4=DPS
  PRIMARY KEY (class, spec_id, item_id)
) ENGINE=InnoDB;

INSERT INTO spec_start_items VALUES
(1,1,20101,4),(1,2,20102,1),(1,3,20103,4),  -- Воин: Осадный/Авангард(танк)/Дуэлянт
(3,4,20301,4),(3,5,20302,4),(3,6,20303,2),  -- Маг: Мороз/Истечения/Свет(хил-поддержка)
(4,7,20401,4),(4,8,20402,4),(4,9,20403,4);  -- Разбойник

-- ---------------------------------------------------------------------
-- 4) XP-кривая: сервер держит формулу в ProgressionCore (детерминизм),
--    в БД — только кэш ориентиров для аналитики/LiveOps (не источник истины!)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS xp_curve_reference (
  level      TINYINT UNSIGNED PRIMARY KEY,
  xp_to_next INT UNSIGNED NOT NULL,
  xp_cumulative BIGINT UNSIGNED NOT NULL
) ENGINE=InnoDB;

INSERT INTO xp_curve_reference (level, xp_to_next, xp_cumulative) VALUES
(1,900,0),
(2,972,900),
(3,1050,1872),
(4,1134,2922),
(5,1225,4056),
(6,1323,5281),
(7,1429,6604),
(8,1544,8033),
(9,1668,9577),
(10,1802,11245),
(11,1947,13047),
(12,2103,14994),
(13,2272,17097),
(14,2454,19369),
(15,2651,21823),
(16,2864,24474),
(17,3094,27338),
(18,3342,30432),
(19,3610,33774),
(20,3899,37384),
(21,4211,41283),
(22,4548,45494),
(23,4912,50042),
(24,5305,54954),
(25,5730,60259),
(26,6189,65989),
(27,6685,72178),
(28,7220,78863),
(29,7798,86083),
(30,8422,93881),
(31,9096,102303),
(32,9824,111399),
(33,10610,121223),
(34,11459,131833),
(35,12376,143292),
(36,13367,155668),
(37,14437,169035),
(38,15592,183472),
(39,16840,199064),
(40,18188,215904),
(41,19644,234092),
(42,21216,253736),
(43,22914,274952),
(44,24748,297866),
(45,26728,322614),
(46,28867,349342),
(47,31177,378209),
(48,33672,409386),
(49,36366,443058),
(50,39276,479424),
(51,42419,518700),
(52,45813,561119),
(53,49479,606932),
(54,53438,656411),
(55,57714,709849),
(56,62332,767563),
(57,67319,829895),
(58,72705,897214),
(59,78522,969919),
(60,84804,1048441),
(61,91589,1133245),
(62,98917,1224834),
(63,106831,1323751),
(64,115378,1430582),
(65,124609,1545960),
(66,134578,1670569),
(67,145345,1805147),
(68,156973,1950492),
(69,169531,2107465),
(70,183094,2276996),
(71,197742,2460090),
(72,213562,2657832),
(73,230647,2871394),
(74,249099,3102041),
(75,269027,3351140),
(76,290550,3620167),
(77,313794,3910717),
(78,338898,4224511),
(79,366010,4563409),
(80,395291,4929419),
(81,426915,5324710),
(82,461069,5751625),
(83,497955,6212694),
(84,537792,6710649),
(85,580816,7248441),
(86,627282,7829257),
(87,677465,8456539),
(88,731663,9134004),
(89,790197,9865667),
(90,853413,10655864),
(91,921687,11509277),
(92,995422,12430964),
(93,1075056,13426386),
(94,1161061,14501442),
(95,1253946,15662503);
