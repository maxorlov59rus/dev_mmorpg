-- =====================================================================
-- «Чёрная Материя» — sql/upgrades/Insert_Quests_6_to_10.sql
-- Артефакт 2 промпта «Квестовая система 6–10: Инстансы и Специализации».
--
-- НУМЕРАЦИЯ: квесты 1001..1007 заняты стартовой линией L1–5
-- (StarterQuests.sql / StarterContent_L1_5.sql). Цепочка 6–10 получает
-- диапазон 1101..1105 — контракт с QuestSystem_Level10.h (enum
-- StarterQuestId6to10) и Lua-скриптами quests/.
--
--   q_1101 (6 ур.) «Первые испытания»    — kill 5x Слизень Истечения (90003)
--   q_1102 (7 ур.) «Доставка донесения»  — travel-goal в соседнюю зону (1201->1100)
--   q_1103 (8 ур.) «Снабжение фронта»    — gather 10x Обломок Этерниума (90003 drop)
--   q_1104 (10 ур.) «Путь избранного»    — выбор специализации (auto-grant ядром)
--   q_1105 (10 ур.) «Проверка боем»      — обучающий инстанс «Пещеры Испытаний» через LFG
--
-- Награды репутации — RAW Q16 (1 очко = 16 ед.), контракт ReputationMgr.
-- Применение: mysql dm_world < Insert_Quests_6_to_10.sql
-- =====================================================================

SET NAMES utf8mb4;

-- ---------------------------------------------------------------------
-- 1) quest_template: цепочка 6–10
-- ---------------------------------------------------------------------
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RewItemId1, RewItemCount1,
   RequiredCreatureKill1, RequiredCreatureKillCount1,
   RequiredSpellFocus1, MinLevel, MaxLevel, ZoneId) VALUES
  (1101, 0, 0, 0, 'Первые испытания',
   'Шестеро слизней у кромки Истечения — это уже не протечка, а вторжение. Командир ополчения считает иначе: зачисти берег, и он посмотрит на тебя как на солдата.',
   'Убейте: Слизень Истечения (5)',
   'Неплохо для новичка. Но убивать слизней — всё равно что вычерпать море ковшом. Дальше будет хуже.',
   'Командир одобряюще кивает.',
   750, 1, 800 /*+50 Стражам Света*/, 2, -480 /*-30 Падшим: трупы их мобов*/,
   0, 0,
   90003, 5,
   0, 6, 9, 1100),
  (1102, 0, 0, 1 /*TRAVEL_GOAL->SpecialFlags bit*/, 'Доставка донесения',
   'Гончая сдохла, а письмо к кузнецу Лагеря Рассвета горит в сумке. Отнеси лично в руки — и не читай по дороге.',
   'Отнесите: Кузнец Таэрн (Лагерь Рассвета, зона 1201)',
   'Таэрн — тот ещё сухарь, но его решётки держат Пологих лучше любых молитв. Передай ему привет от меня.',
   'Кузец надменно вскрывает печать.',
   900, 1, 640 /*+40*/, 2, 0,
   0, 0,
   0, 0,
   0, 7, 9, 1100),
  (1103, 0, 0, 0, 'Снабжение фронта',
   'Фронт у Порога жрёт этерниум тоннами: решётки, клетки, цепи. Десять обломков — капля, но из капель состоят фронты.',
   'Соберите: Обломок Этерниума (10)',
   'Десять? Из этого выйдет одна приличная цепь. Одна цепь — один пойманный Полый. Считай, спас деревню.',
   'Интендант грузит обломки на телегу.',
   1200, 1, 1280 /*+80*/, 2, -640 /*-40*/,
   0, 0,
   90003, 10, -- gather через дроп с тех же слизней (quest_drop_data ниже)
   0, 8, 9, 1201),
  (1104, 0, 0, 4 /*HIDDEN_TRACKER + auto-grant ядром SpecQuestMgr*/, 'Путь избранного',
   'Тебе десять. Пора выбирать: кем ты будешь, когда Чёрная Материя придёт за тобой всерьёз. Щитом или клинком — но выбором, а не случайностью.',
   'Выберите специализацию (диалог с наставником)',
   'Звёзды смотрят на тебя. Не подведи их — они заметят.',
   'Наставник ставит клеймо на твоей ладони — знак выбранного пути.',
   1500, 1, 1600 /*+100*/, 2, -800 /*-50*/,
   0, 0,
   0, 0,
   0, 10, 10, 1100),
  (1105, 0, 0, 0, 'Проверка боем',
   'Слова о выборе — красивая мишура. Настоящий путь покажут Пещеры Испытаний: найди группу через Рупор призыва и дойди до сердца пещеры. В одиночку туда не ходят.',
   'Пройдите инстанс «Пещеры Испытаний» (LFG), убейте: Полой Вождь Пещеры',
   'Изгой бы сдох там в первые десять минут. А ты — нет. Значит, рупор я дал не зря.',
   'Командир выдаёт Рупор призыва и хмурится: «В следующий раз зови сам.»',
   2000, 1, 2400 /*+150*/, 2, -1200 /*-75*/,
   5120, 1, -- «Рупор призыва» (расходник-жетон, см. item_template ниже)
   0, 0,
   0, 10, 15, 901);

-- ---------------------------------------------------------------------
-- 2) quest_drop_data: гарантированный квестовый предмет для gather-цели q_1103
--    (паттерн StarterContent_L1_5.sql §quest_drop_data)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS quest_drop_data (
  quest_id       MEDIUMINT UNSIGNED NOT NULL,
  creature_entry INT UNSIGNED NOT NULL,
  item_entry     INT UNSIGNED NOT NULL,
  chance_pct     TINYINT UNSIGNED NOT NULL DEFAULT 100,
  PRIMARY KEY (quest_id, creature_entry, item_entry)
) ENGINE=InnoDB;

INSERT INTO quest_drop_data VALUES
  (1103, 90003, 5001, 100);  -- Обломок Этерниума со слизней — строго по квесту

-- Предмет цели q_1103 (если ещё не создан в L1-5 контенте — идемпотентно)
INSERT IGNORE INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank, Material) VALUES
  (5001, 1, 0, 'Обломок Этерниума', 5001, 1, 8, 0, -1, -1, 0 /*loose*/, 0, 0, 1 /*metal*/);

-- «Рупор призыва» — расходный жетон LFG (effect -> CMSG_JOIN_LFG dungeon=1)
INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank, Material, SpellID_1) VALUES
  (5120, 4 /*item class: consumable-аналог TRC*, subclass 0*/, 0, 'Рупор призыва', 5120, 3, 10, 10,
   -1, -1, 1 /*BoP*/, 0, 0, 1, 110501 /*use-effect: join LFG queue*/);

-- ---------------------------------------------------------------------
-- 3) Цепочки и привязка к NPC
-- ---------------------------------------------------------------------
-- 1101->1102->1103 (обе фракции через своих наставников), 1103->1104 (наставник 10 lvl),
-- 1104->1105 (после выбора спеки — auto via Lua OnComplete + gossip option 2)
INSERT INTO quest_chain VALUES
  (1101, 1102, 90021), (1102, 1103, 90001),
  (1103, 1104, 90021), (1104, 1105, 90021),
  -- зеркало для Ордена Пепла (свои entry'и наставников)
  (1101, 1102, 90022), (1102, 1103, 90002),
  (1103, 1104, 90022), (1104, 1105, 90022);

INSERT INTO creature_quest_relation VALUES
  (90021, 1101, 'start'), (90021, 1101, 'end'),
  (90021, 1102, 'end'),
  (90001, 1103, 'end'),
  (90021, 1104, 'start'), (90021, 1104, 'end'),
  (90021, 1105, 'start'), (90021, 1105, 'end'),
  (90022, 1101, 'start'), (90022, 1101, 'end'),
  (90022, 1102, 'end'),
  (90002, 1103, 'end'),
  (90022, 1104, 'start'), (90022, 1104, 'end'),
  (90022, 1105, 'start'), (90022, 1105, 'end');

-- ---------------------------------------------------------------------
-- 4) NPC наставники 10 уровня (creature_template + spawn + gossip)
-- ---------------------------------------------------------------------
INSERT INTO creature_template (entry, name, faction_A, faction_H, minlevel, maxlevel, unit_flags, type) VALUES
  (90021, 'Командир Ополчения', 1, 1, 10, 12, 0x0004 /*NOT_ATTACKABLE*/, 7 /*npc*/),
  (90022, 'Смотрящая за Пеплом', 2, 2, 10, 12, 0x0004, 7);

-- Gossip-меню выбора специализации: 110400+class (контракт npc_quest_giver_10.lua).
-- Опции скрыты до 10 уровня через condition_id (dm_conditions kind='min_level').
INSERT INTO dm_conditions (id, kind, param1, param2) VALUES
  (1104, 'min_level', 10, 0),
  (1105, 'has_spec', 0, 0),    -- spec отсутствует
  (1106, 'has_spec', 1, 0);    -- spec выбрана (для LFG-ветки)
INSERT INTO gossip_menu_option (MenuID, ID, OptionIcon, OptionText, OptionBroadcastTextID, OptionType, Action, condition_id) VALUES
  (110401, 1, 1 /*quest!*/, 'Покажи мне путь Осадного.',   90040, 1, 2, 1104),
  (110401, 2, 1, 'Я буду стоять стеной — Авангард.',        90041, 1, 2, 1104),
  (110401, 3, 1, 'Клинок выбирает меня — Дуэлянт.',          90042, 1, 2, 1104),
  (110403, 1, 1, 'Мороз послушен мне.',                      90043, 1, 2, 1104),
  (110403, 2, 1, 'Я иду путём Истечения.',                   90044, 1, 2, 1104),
  (110403, 3, 1, 'Свет лечит то, что я сломал.',             90045, 1, 2, 1104),
  (110404, 1, 1, 'Тень — мой второй силуэт.',                90046, 1, 2, 1104),
  (110404, 2, 1, 'Яд честнее чести.',                        90047, 1, 2, 1104),
  (110404, 3, 1, 'Порог открывает двери тем, кто бежит.',    90048, 1, 2, 1104),
  -- LFG-ветка после выбора спеки (menu 110500)
  (110500, 1, 0 /*talk*/, 'Дай мне Рупор призыва.',          90049, 1, 2, 1106);

INSERT INTO broadcast_text VALUES
  (90030, 'Командир Ополчения: «Возвращайся, когда станешь сильнее. Десять зим — и приходи.»', 0),
  (90031, 'Командир Ополчения: «Ты уже в пути. Не сворачивай.»', 0),
  (90032, 'Командир Ополчения: «Звёзды смотрят на тебя.»', 0),
  (90033, 'Командир Ополчения: «Протруби в него у костра. Тебя услышат.»', 0),
  (90034, 'Командир Ополчения: «Рупор уже у тебя. Труби.»', 0);

-- ---------------------------------------------------------------------
-- 5) playercreateinfo_specs: способности специализаций (контракт
--    SpecQuestMgr::loadFromDb + quest_1004_spec_choice.lua SPECS_BY_CLASS)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS playercreateinfo_specs (
  class_id   TINYINT UNSIGNED NOT NULL,
  spec_id    TINYINT UNSIGNED NOT NULL,
  spell_id_1 INT UNSIGNED NOT NULL DEFAULT 0,
  spell_id_2 INT UNSIGNED NOT NULL DEFAULT 0,
  spell_id_3 INT UNSIGNED NOT NULL DEFAULT 0,
  spell_id_4 INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (class_id, spec_id),
  CONSTRAINT fk_pcs_class CHECK (class_id BETWEEN 1 AND 6),
  CONSTRAINT fk_pcs_spec  CHECK (spec_id BETWEEN 1 AND 9)
) ENGINE=InnoDB;

INSERT INTO playercreateinfo_specs (class_id, spec_id, spell_id_1, spell_id_2, spell_id_3, spell_id_4) VALUES
 -- Воин: Осадный (STR) — раскалывание щитов/замедления
 (1, 1, 11101, 11102, 11103, 0),
 -- Воин: Авангард (STA, ТАНК) — угроза/блок/временные HP от Порчи
 (1, 2, 11201, 11202, 11203, 11204),
 -- Воин: Дуэлянт (AGI) — крит/контрудар
 (1, 3, 11301, 11302, 11303, 0),
 -- Маг: Мороз (INT) — ледяные кристаллы Niagara
 (3, 4, 13401, 13402, 13403, 0),
 -- Маг: Истечения (INT) — Школа Чёрной Материи
 (3, 5, 13501, 13502, 13503, 13504),
 -- Маг: Свет (INT, HEALER) — лечение сквозь контроль
 (3, 6, 13601, 13602, 13603, 0),
 -- Разбойник: Тень (AGI) — afterimage-спринт
 (4, 7, 14701, 14702, 14703, 0),
 -- Разбойник: Послушник (AGI) — яды
 (4, 8, 14801, 14802, 14803, 0),
 -- Разбойник: Порожник (AGI) — рывки/портальные кувырки
 (4, 9, 14901, 14902, 14903, 0);

-- ---------------------------------------------------------------------
-- 6) lfg_dungeons: инстансы очереди (контракт LFGMgr::loadFromDb)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lfg_dungeons (
  ID         SMALLINT UNSIGNED NOT NULL PRIMARY KEY,
  NameRu     VARCHAR(96) NOT NULL,
  MapID      INT UNSIGNED NOT NULL,
  MinLevel   TINYINT UNSIGNED NOT NULL DEFAULT 10,
  MaxLevel   TINYINT UNSIGNED NOT NULL DEFAULT 15,
  RequiredRolesMask TINYINT UNSIGNED NOT NULL DEFAULT 7 COMMENT 'биты tank|healer|dps (LfgRoleFlags)',
  GroupSize  TINYINT UNSIGNED NOT NULL DEFAULT 4,
  KEY idx_lfg_level (MinLevel, MaxLevel)
) ENGINE=InnoDB;

INSERT INTO lfg_dungeons (ID, NameRu, MapID, MinLevel, MaxLevel, RequiredRolesMask, GroupSize) VALUES
  (1, 'Пещеры Испытаний', 901, 10, 15, 7, 4);   -- обучающий, цель босса = goal q_1105

-- Area-trigger прогресса квеста 1105: вход в инстанс засчитывает travel-цель
INSERT INTO area_trigger_state (trigger_id, zone_script) VALUES
  (9010, 'quest_complete_progress:1105:enter_instance');

-- ---------------------------------------------------------------------
-- 7) Дроп мобов цепочки (reference_loot): слизни 6–9 уровня дают ресурсы
--    q_1103 + валюту осколков; босс пещеры — синий лут (спеку-токин-нет:
--    first-clear награда квестом, не лутом — баланс docs/05)
-- ---------------------------------------------------------------------
INSERT INTO reference_loot_template (Entry, Item, ChanceOrQuestChance, GroupID, Comment) VALUES
  (301, 5001, 20, 0, 'Этерниум со слизня (фарм-база)'),
  (301, 5002, 8, 0, 'Осколок Порога (валюта Гильдии)'),
  (302, 5001, 35, 0, 'Зона Истечения: усиленный дроп для изгоев');

UPDATE creature_template SET lootid = 301 WHERE entry = 90003;

-- Босс обучающего инстанса (spawn в map 901 — позиционный дамп в M2)
INSERT INTO creature_template (entry, name, faction_A, faction_H, minlevel, maxlevel, unit_flags, type, lootid) VALUES
  (90030, 'Полой Вождь Пещеры', 1650, 1650, 10, 10, 0, 6 /*elite*/, 301);
