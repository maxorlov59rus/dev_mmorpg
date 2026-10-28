-- =====================================================================
-- «Чёрная Материя» — StarterContent_L1_5.sql (dm_world, контент v3)
-- Уровни 1–5: диалоги мобов (broadcast_text), реплики квестодавателей,
-- дроп по заданиям/квестам (loot-таблицы), служебные NPC на стартовых
-- картах обеих фракций.
--
-- Соглашения id (расширение StarterQuests.sql):
--   quests        1001..1009  (1001-1003 уже заняты стартовыми)
--   creature     90000+       | items 5000+ | areas 1100/1200/1400 (уже есть)
--   new areas:    1101 Лагерь Рассвета, 1201 Пепельный Карьер
--   broadcast  90000..90999   — стартовый контент (этот файл)
--                91000..91999 — реплики мобов lvl 1-5
--                92000..92999 — реплики служебных NPC (trainer/vendor/healer)
--   gossip_menu   5000..5099  — стартовые NPC (5001-5003 заняты)
--   loot          300..399    — существующие: 300 Слизень; новые ниже
--   vendor_rec    700..       — товарные списки служебных NPC
-- =====================================================================

USE dm_world;

-- ---------------------------------------------------------------------
-- 0) ЛОГ ДИАЛОГОВ (создан в NPC_Config.sql; IF NOT EXISTS для идемпотентности)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS broadcast_text (
  ID MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  Text0 TEXT NOT NULL, LanguageID TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- 1) ДИАЛОГИ МОБОВ LVL 1-5 (аггро/ранение/смерть).
--    Механика: SmartAI event DEAGGRO/HP_PERCENT/DEATH -> action CREATURE_TEXT
--    (SMSG_CREATURE_TEXT 0x2014). Тексты аггро у мобов Ордена Пепла
--    учитывают, что их бьют в основном послушники Стражей (q_1002/q_1005).
-- ---------------------------------------------------------------------
INSERT INTO broadcast_text VALUES
  -- 90003 Слизень Истечения (lvl 3-4, цель q_1002 x5)
  (91001, 'Слизень булькает, вытягивая осколок из земли.', 0),
  (91002, 'Слизень вздрагивает — трещины на его теле светятся.', 0),
  (91003, 'Слизень лопается, рассыпаясь чёрной слизью.', 0),
  -- 90010 Пепельный Жук (lvl 1-2, Орден Пепла, зона 1201)
  (91011, 'Жук скрежещет панцирем и бросается вперёд.', 0),
  (91012, 'Панцирь жука раскалывается, из-под него валит пепел.', 0),
  (91013, 'Жук замирает, догорая изнутри.', 0),
  -- 90011 Молодой Полый Охотник (lvl 4-5, elite-редкий спавн)
  (91021, 'ПОЛЫЙ! Его крик похож на рвущуюся ткань мира.', 0),
  (91022, 'Охотник рычит — в его груди вместо сердца воронка.', 0),
  (91023, 'Охотник оседает; воронка схлопывается с хлопком.', 0),
  -- 90012 Тень Истечения (lvl 5, нейтрально-злая, зона 1400 мост Гримма)
  (91031, 'Тень шипит: «Здесь нет флагов. Здесь нет выхода.»', 0),
  (91032, 'Тень теряет плотность и мечется.', 0),
  (91033, 'Тень растворяется в свете факелов.', 0);

-- Привязка текстов к мобам: расширяем creature_template колонками aggro/death text id
ALTER TABLE creature_template
  ADD COLUMN AggroTextID   MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN WoundTextID   MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN DeathTextID   MEDIUMINT UNSIGNED NOT NULL DEFAULT 0;

UPDATE creature_template SET AggroTextID=91001, WoundTextID=91002, DeathTextID=91003 WHERE entry=90003; -- Слизень
UPDATE creature_template SET AggroTextID=91011, WoundTextID=91012, DeathTextID=91013 WHERE entry=90010; -- Жук
UPDATE creature_template SET AggroTextID=91021, WoundTextID=91022, DeathTextID=91023 WHERE entry=90011; -- Полый охотник
UPDATE creature_template SET AggroTextID=91031, WoundTextID=91032, DeathTextID=91033 WHERE entry=90012; -- Тень

-- ---------------------------------------------------------------------
-- 2) НОВЫЕ МОБЫ LVL 1-5 (кроме уже существующих 90003 Слизень)
--    faction_A/H: 0x1 = Стражи Света, 0x2 = Падшие (как в StarterQuests).
--    lootid: 300 Слизень (уже), 301 жук, 302 полый, 303 тень.
-- ---------------------------------------------------------------------
INSERT INTO creature_template (entry, name, subname, min_level, max_level,
   faction_A, faction_H, unit_flags, unit_flags2, lootid, script_name) VALUES
  (90010, 'Пепельный Жук', '', 1, 2, 0, 0x0002, 0, 0, 301, ''),
  (90011, 'Молодой Полый Охотник', 'Пожирающий осколки', 4, 5, 0, 0x0002, 0, 0, 302, 'mob_hollow_elite'),
  (90012, 'Тень Истечения', '', 5, 5, 0, 0, 0, 0, 303, '');

-- ---------------------------------------------------------------------
-- 3) ДРОП ПО ЗАДАНИЯМ/КВЕСТАМ
--    a) Квестовые предметы дропаются целевым мобом с Chance=100 (конвейер квеста).
--    b) reference_loot_template + zone_loot_rule — обычный зонный дроп.
-- ---------------------------------------------------------------------

-- Предметы заданий (class=1 quest-analog, Bonding=3 quest)
INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank, Material) VALUES
  (5302, 1, 0, 'Ядро слизня (свежее)',        13302, 1, 1, 1, -1, -1, 3 /*quest*/, 0, 0, 0), -- хвост q_1002 (5 шт)
  (5303, 1, 0, 'Обломок Этерниума',           13303, 2, 2, 1, -1, -1, 3, 0, 0, 0),           -- q_1005 x8
  (5304, 1, 0, 'Панцирь Пепельного Жука',     13304, 1, 1, 1, -1, -1, 3, 0, 0, 0),           -- q_1004 x6
  (5305, 1, 0, 'Выжженное Сердце Полого',     13305, 3, 4, 4, -1, -1, 3, 0, 0, 0),           -- q_1006 (1)
  (5306, 1, 0, 'Запечатанный Сверток Порога', 13306, 3, 1, 5, -1, -1, 3, 0, 0, 0);           -- q_1007 (1)

-- a) Гарантированный квестовый дроп: отдельная таблица привязки
--    (не смешиваем с рандомным лутом — TRC делает так же через dedicated quest loot)
CREATE TABLE IF NOT EXISTS quest_drop_data (
  CreatureEntry MEDIUMINT UNSIGNED NOT NULL,
  QuestID       MEDIUMINT UNSIGNED NOT NULL,
  ItemID        MEDIUMINT UNSIGNED NOT NULL,
  Count         SMALLINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (CreatureEntry, QuestID, ItemID)
) ENGINE=InnoDB;

INSERT INTO quest_drop_data VALUES
  (90003, 1002, 5302, 1),   -- Слизень -> ядро (нужно 5)
  (90010, 1004, 5304, 1),   -- Жук     -> панцирь (нужно 6)
  (90011, 1006, 5305, 1),   -- Полый   -> сердце (1, drop-chance через elite-pool ниже)
  (90012, 1007, 5306, 1);   -- Тень    -> сверток (1)

-- b) Зонный лут мобов lvl 1-5 (reference-таблицы, Chance в % x10 => 1000=100%)
CREATE TABLE IF NOT EXISTS loot_template (
  LootId     SMALLINT UNSIGNED NOT NULL,
  ItemOrRef  MEDIUMINT UNSIGNED NOT NULL,
  IsReference TINYINT UNSIGNED NOT NULL DEFAULT 1,
  Chance     INT UNSIGNED NOT NULL COMMENT '% x10',
  MinCount   TINYINT UNSIGNED NOT NULL DEFAULT 1,
  MaxCount   TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (LootId, ItemOrRef)
) ENGINE=InnoDB;

INSERT INTO loot_template VALUES
  (300, 5001, 0, 400, 1, 1),      -- Слизень: самородок Этерниум 40%
  (300, 5310, 0, 1000, 1, 2),     -- ...и всегда: слизистая жижа (крафт-мусор)
  (301, 5001, 0, 250, 1, 1),      -- Жук: Этерниум 25%
  (301, 5311, 0, 1000, 1, 1),     -- оплавленный усик (крафт)
  (302, 302, 1, 1000, 1, 1),      -- Полый: ссылается на ref-пул 302 ниже
  (303, 5312, 0, 600, 1, 1);      -- Тень: сгусток тени (крафт)

INSERT INTO reference_loot_template (Entry, Item, Chance, MinCount, MaxCount) VALUES
  (302, 5001, 1000, 2, 3),        -- guaranteed 2-3 этерниума
  (302, 5313, 300, 1, 1),         -- редкий «осколок ядра» 30%
  (302, 5305, 200, 1, 1);         -- выжженное сердце 20% (плюс quest-drop конвейер)

-- Крафт-мусор предметов ещё нет в item_template — добавим
INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank) VALUES
  (5310, 6, 0, 'Слизистая Жижа',       15310, 1, 1, 1, -1, -1, 0, 0, 0),
  (5311, 6, 0, 'Оплавленный Усик',     15311, 1, 1, 1, -1, -1, 0, 0, 0),
  (5312, 6, 0, 'Сгусток Тени',         15312, 1, 1, 1, -1, -1, 0, 0, 0),
  (5313, 6, 0, 'Осколок Ядра',         15313, 2, 3, 1, -1, -1, 0, 0, 0);

-- Зонное правило: в зоне Истечения (1400) мобы дропают ЧЁРНЫЙ лут ренегатам
-- (zone_loot_rule уже создана в DB_Schema.sql — заполняем данные)
INSERT INTO zone_loot_rule (ZoneId, RuleType, Param, ReferenceId) VALUES
  (1400, 'renegade_bonus_umbra', 1, 302);  -- изгои получают доп. пул осколков

-- ---------------------------------------------------------------------
-- 4) КВЕСТЫ LVL 1-5 (продолжение стартовых линий до 5 уровня)
-- ---------------------------------------------------------------------
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RewItemId1, RewItemCount1,
   RequiredCreatureKill1, RequiredCreatureKillCount1, MinLevel, MaxLevel, ZoneId) VALUES
  (1004, 0, 0, 0, 'Жуки в карьере',
   'Пепельный карьер заражён жуками — они разносят слизь Истечения быстрее нас. Собери шесть целых панцирей: из них делают фильтры для масок.',
   'Соберите: Панцирь Пепельного Жука (6)',
   'Шесть панцирей — шесть спасённых лёгких. Считай это первым долгом Ордена перед тобой.',
   'Инженерша Пепла пересчитывает панцири.',
   500, 1, -160 /*-10 Стражам*/, 2, 800 /*+50 Падшим*/,
   0, 0, 90010, 6, 1, 5, 1201),
  (1005, 0, 0, 0, 'Этерниум для кузни',
   'Кузне Лагеря Рассвета нужен этерниум. Разведчики клянчат его у слизней устья — принеси восемь обломков.',
   'Соберите: Обломок Этерниума (8)',
   'Из этого металла куют решётки для клеток Полого. Ирония? Нет — необходимость.',
   'Кузнец взвешивает обломки на руке.',
   500, 1, 800 /*+50 Стражам*/, 2, -160,
   0, 0, 90003, 8, 2, 5, 1100);

-- q_1006 и q_1007 используют talk/kill комбо + авто-цепочку; цели — элита и тень
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RewItemId1, RewItemCount1,
   RequiredCreatureKill1, RequiredCreatureKillCount1, MinLevel, MaxLevel, ZoneId) VALUES
  (1006, 0, 0, 0, 'Воронка в груди',
   'Молодой Полый охотник гнездится у старого моста. Вырви его выжженное сердце, пока воронка не разрослась — такие, повзрослев, открывают Истечения сами.',
   'Убейте: Молодой Полый Охотник (1), заберите: Выжженное Сердце Полого',
   'Ты видел воронку изнутри и остался собой. Запомни это ощущение — оно тебе ещё пригодится.',
   'Охотница Рассвета закапывает сердце в соль.',
   800, 1, 1200 /*+75*/, 2, -400 /*-25*/,
   5104, 1, 90011, 1, 4, 5, 1100),
  (1007, 0, 0, 0, 'Послание без знамени',
   'На мосту Гримма водятся Тени Истечения — они собирают чужие слова. Забери у одной из них запечатанный свиток: он не должен дойти ни до одного из двух знамён.',
   'Убейте: Тень Истечения (1), заберите: Запечатанный Сверток Порога',
   'Свиток порога? Ты даже не представляешь, кому его нельзя отдавать. Мне, например. Оставь при себе.',
   'Гримм не берёт свиток — только нюхает.',
   1000, 1, 0, 2, 0,
   0, 0, 90012, 1, 5, 5, 1400);

-- Награда q_1006 — плащ Стражей (новый предмет)
INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank, Material) VALUES
  (5104, 4, 4, 'Плащ Охотницы на Полого', 25104, 2, 5, 4, -1, -1, 1 /*BoP*/, 1, 0, 2 /*leather*/);

-- Цепочки: 1001->1005 (Стражи), 1002->1004 (Пепел), 1005->1006, 1006/1004->1007(нейтр., у Гримма)
CREATE TABLE IF NOT EXISTS quest_chain (
  PrevQuest  MEDIUMINT UNSIGNED NOT NULL,
  NextQuest  MEDIUMINT UNSIGNED NOT NULL,
  NpcEntry   MEDIUMINT UNSIGNED NOT NULL COMMENT 'кто предлагает продолжение',
  PRIMARY KEY (PrevQuest, NextQuest)
) ENGINE=InnoDB;
INSERT INTO quest_chain VALUES
  (1001, 1005, 90001), (1005, 1006, 90001),
  (1002, 1004, 90002), (1004, 1007, 90004), (1006, 1007, 90004);

-- Привязка новых квестов к NPC
INSERT INTO creature_quest_relation VALUES
  (90001, 1005, 'start'), (90001, 1005, 'end'),
  (90001, 1006, 'start'), (90001, 1006, 'end'),
  (90002, 1004, 'start'), (90002, 1004, 'end'),
  (90004, 1007, 'start'), (90004, 1007, 'end');

-- Реплики квестодавателей (Offer/Complete озвучка через OptionBroadcastTextID)
INSERT INTO broadcast_text VALUES
  (90010, 'Веларион: «Граница дрожит. Отнеси свиток — и не задерживайся на пути.»', 0),
  (90011, 'Веларион: «Карьер под нашим лагерем гудит. Там жуки. Нам нужны панцири — но пусть это сделает Пепел: у них маски лучше.»', 0),
  (90012, 'Морвен: «Пепел помнит всех, кто копал в этом карьере. Теперь там копошатся жуки. Шесть панцирей — и ты свой.»', 0),
  (90013, 'Гримм: «Мне? Да. Принеси то, что тени собрали у моста. Деньги пахнут одинаково под любым знаменем.»', 0);

-- ---------------------------------------------------------------------
-- 5) СЛУЖЕБНЫЕ NPC LVL 1-5 (на стороне сервера — entries+script_name;
--    клиент читает dbc-экспорт creature_template + DataTable DM_ServiceNPCs)
-- ---------------------------------------------------------------------
INSERT INTO creature_template (entry, name, subname, min_level, max_level,
   faction_A, faction_H, unit_flags, unit_flags2, lootid, script_name) VALUES
  (90020, 'Тренер Мечника Алдис',   'Учитель боя',        5, 5, 0x0001, 0, 0, 0, 0, 'npc_trainer_melee'),
  (90021, 'Тренер Послушница Кейл', 'Учитель магии',      5, 5, 0x0001, 0, 0, 0, 0, 'npc_trainer_cast'),
  (90022, 'Травница Лисанна',       'Зелья и еда',        5, 5, 0x0001, 0, 0, 0, 0, 'npc_vendor_consumables'),
  (90023, 'Светильщица Марра',      'Целитель у костра',  8, 8, 0x0001, 0, 0, 0, 0, 'npc_healer_light'),
  (90024, 'Банкир Одо',             'Хранилище',          5, 5, 0x0001, 0, 0, 0, 0, 'npc_banker'),
  (90030, 'Тренер Пепельный Клинок','Учитель боя',        5, 5, 0, 0x0002, 0, 0, 0, 'npc_trainer_melee'),
  (90031, 'Тренерша Угли',          'Учитель магии',      5, 5, 0, 0x0002, 0, 0, 0, 'npc_trainer_cast'),
  (90032, 'Менесток',               'Зелья и еда',        5, 5, 0, 0x0002, 0, 0, 0, 'npc_vendor_consumables'),
  (90033, 'Пепельный Целитель',     'Жрец углей',         8, 8, 0, 0x0002, 0, 0, 0, 'npc_healer_fallen'),
  (90034, 'Скупщик',                'Продажа хлама',      5, 5, 0, 0x0002, 0, 0, 0, 'npc_vendor_trash'),
  (90040, 'Вестовой Порога',        'Нейтральный почтамт',10,10, 0, 0, 0, 0, 0, 'npc_mailbox_neutral');

-- Госсипы служебных NPC (ActionType: 1 vendor, 2 quest-menu, 6 trainer, 8 banker/heal)
INSERT INTO gossip_menu (ID, TextID, ScriptName) VALUES
  (5010, 92001, 'gossip_trainer'),      -- оба тренера melee/cast используют общую логику
  (5011, 92002, 'gossip_vendor'),
  (5012, 92003, 'gossip_healer'),
  (5013, 92004, 'gossip_banker'),
  (5014, 92005, 'gossip_mailbox');

INSERT INTO gossip_menu_option (MenuID, ID, OptionIcon, OptionText, OptionBroadcastTextID, OptionType, Action, condition_id) VALUES
  (5010, 1, 6, 'Обучи меня.',                       92010, 6 /*trainer*/, 6, 3 /*not_renegade*/),
  (5010, 2, 0, 'Что ты знаешь о Чёрной Материи?',   92011, 0, 0, 3),
  (5011, 1, 3, 'Твой товар.',                       92020, 2 /*trade*/, 1 /*vendor*/, 3),
  (5011, 2, 3, 'Купи у меня лишнее.',               92021, 2, 1, 3),
  (5012, 1, 0, 'Мне нужна помощь.',                 92030, 0, 7 /*heal*/, 3),
  (5012, 2, 0, 'Отвести меня к костру.',            92031, 0, 0, 3),
  (5013, 1, 4, 'Открой хранилище.',                 92040, 4 /*banker*/, 8, 3),
  (5014, 1, 0, 'Отправить письмо.',                 92050, 0, 9 /*mailbox*/, 0 /*всем, даже изгоям*/);

UPDATE creature_template SET gossip_menu_id = 5010 WHERE entry IN (90020,90021,90030,90031);
UPDATE creature_template SET gossip_menu_id = 5011 WHERE entry IN (90022,90032,90034);
UPDATE creature_template SET gossip_menu_id = 5012 WHERE entry IN (90023,90033);
UPDATE creature_template SET gossip_menu_id = 5013 WHERE entry = 90024;
UPDATE creature_template SET gossip_menu_id = 5014 WHERE entry = 90040;

-- Реплики служебных NPC
INSERT INTO broadcast_text VALUES
  (92001, '«Стоять смирно. Клинок — продолжение воли, а не руки.»', 0),
  (92002, '«Деньги вперёд. Совести — потом, если останется место.»', 0),
  (92003, '«Свет не спрашивает, достойны ли вы его. Просто держись у костра.»', 0),
  (92004, '«Твоё не потеряется. Даже если ты сам его потеряешь.»', 0),
  (92005, '«Порог доставляет всё. Кроме обещаний.»', 0),
  (92010, 'Тренер: «Покажи, чему научился за прошлый раз.»', 0),
  (92011, 'Тренер: «Чёрная Материя — не враг. Враг — думать, что её можно понять.»', 0),
  (92020, 'Менесток: «Зелье от слизи, хлеб от голода, верёвка от глупости.»', 0),
  (92021, 'Менесток: «Хлам? Для меня — товар. Для тебя — урок.»', 0),
  (92030, 'Целитель: «Держись. Боль — это просто память тела.»', 0),
  (92031, 'Целитель: «У огня безопаснее. Садись.»', 0),
  (92040, 'Одо: «Пять монет за неделю хранения. Шестая — за болтовню.»', 0),
  (92050, 'Вестовой: «Индекс зоны. Сургуч нейтрален.»', 0);

-- Товарные списки служебных NPC (consumables/vendors)
CREATE TABLE IF NOT EXISTS vendor_rec (
  VendorId  MEDIUMINT UNSIGNED NOT NULL,
  ItemId    MEDIUMINT UNSIGNED NOT NULL,
  Price     INT UNSIGNED NOT NULL COMMENT 'медь',
  Slot      TINYINT UNSIGNED NOT NULL,
  StockMax  SMALLINT NOT NULL DEFAULT -1,
  ReqRenegade TINYINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (VendorId, ItemId)
) ENGINE=InnoDB;

INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank) VALUES
  (5501, 4, 0, 'Лёгкая кольчуга Послушника', 25501, 1, 3, 1, -1, -1, 0, 0, 0),
  (5502, 4, 0, 'Штаны Углекопа',             25502, 1, 3, 1, -1, -1, 0, 0, 0),
  (5503, 0, 0, 'Топорик Стажёра',            25503, 1, 3, 1, -1, -1, 0, 0, 0),
  (5601, 6, 0, 'Зелье Малого Исцеления',     15601, 1, 1, 1, -1, -1, 0, 0, 0),
  (5602, 6, 0, 'Хлеб Ночлежки',              15602, 1, 1, 1, -1, -1, 0, 0, 0),
  (5603, 6, 0, 'Фляга Воды Истечения',       15603, 1, 1, 1, -1, -1, 0, 0, 0),
  (5604, 6, 0, 'Маска с Фильтром из Панциря',15604, 1, 1, 1, -1, -1, 0, 0, 0);

INSERT INTO vendor_rec VALUES
  -- consumables (общий прайс, обе стороны)
  (90022, 5601, 250, 0, -1, 0), (90022, 5602, 80, 1, -1, 0), (90022, 5603, 60, 2, -1, 0),
  (90032, 5601, 250, 0, -1, 0), (90032, 5602, 80, 1, -1, 0), (90032, 5604, 300, 2, -1, 0),
  -- базовое снаряжение
  (90022, 5501, 900, 3, 20, 0), (90022, 5503, 1200, 4, 10, 0),
  (90032, 5502, 900, 3, 20, 0), (90032, 5503, 1200, 4, 10, 0),
  -- скупщик хлама покупает крафт-мусор (Price = buy price)
  (90034, 5310, 20, 0, -1, 0), (90034, 5311, 25, 1, -1, 0), (90034, 5312, 35, 2, -1, 0),
  -- Чёрный рынок Гримма (только изгои) — тот же товар дешевле, но с риском
  (90004, 5601, 180, 0, 50, 1), (90004, 5503, 800, 1, 5, 1);

-- Стартовые спавны служебных NPC и мобов lvl 1-5 (карты стартовых зон)
CREATE TABLE IF NOT EXISTS creature_spawn (
  SpawnGuid BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  Entry     MEDIUMINT UNSIGNED NOT NULL,
  MapId     SMALLINT UNSIGNED NOT NULL,
  X INT NOT NULL, Y INT NOT NULL, Z INT NOT NULL COMMENT 'Q16.16 fixed',
  O INT NOT NULL DEFAULT 0 COMMENT 'yaw Q16.16',
  CurHealth INT NOT NULL DEFAULT 0,
  InRespawn TINYINT NOT NULL DEFAULT 0,
  SpawnsMask INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'бит0=always бит1=renegade-only бит2=factionA бит3=factionB'
) ENGINE=InnoDB AUTO_INCREMENT=200000;

-- Карта 1100 (Лагерь Рассвета): тренеры/торговец/целитель/банкир вокруг командира
INSERT INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
  (90020, 1100, 65536*12, 65536*8,  65536*2, 0x5),
  (90021, 1100, 65536*13, 65536*8,  65536*2, 0x5),
  (90022, 1100, 65536*12, 65536*10, 65536*2, 0x5),
  (90023, 1100, 65536*11, 65536*9,  65536*2, 0x5),
  (90024, 1100, 65536*12, 65536*11, 65536*2, 0x5),
  (90003, 1100, 65536*30, 65536*22, 65536*1, 0x1),   -- слизни устья (за пределами лагеря)
  (90011, 1100, 65536*40, 65536*25, 65536*1, 0x1);   -- элитный полый у моста (q_1006)
-- Карта 1201 (Пепельный Карьер)
INSERT INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
  (90030, 1201, 65536*9,  65536*7,  65536*3, 0x6),
  (90031, 1201, 65536*10, 65536*7,  65536*3, 0x6),
  (90032, 1201, 65536*9,  65536*9,  65536*3, 0x6),
  (90033, 1201, 65536*8,  65536*8,  65536*3, 0x6),
  (90034, 1201, 65536*9,  65536*10, 65536*3, 0x6),
  (90010, 1201, 65536*20, 65536*15, 65536*2, 0x1),   -- жуки карьера (q_1004)
  (90003, 1201, 65536*25, 65536*18, 65536*2, 0x1);
-- Карта 1400 (Мост Гримма, нейтральные земли)
INSERT INTO creature_spawn (Entry, MapId, X, Y, Z, SpawnsMask) VALUES
  (90004, 1400, 65536*5,  65536*5,  65536*0, 0x1),   -- контрабандист (всегда)
  (90040, 1400, 65536*6,  65536*5,  65536*0, 0x1),   -- почтамт Порога
  (90012, 1400, 65536*15, 65536*12, 65536*0, 0x1);   -- тени моста (q_1007)

-- ---------------------------------------------------------------------
-- 6) CLIENT-SIDE КОНТРАКТЫ (UE DataTable → экспорт в .dbc/pak, docs/04 §3)
-- ---------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS client_dbc_export_map (
  TableSource VARCHAR(64) NOT NULL,
  DbcName     VARCHAR(32) NOT NULL,
  ClientDataTable VARCHAR(64) NOT NULL COMMENT 'UE DataTable asset path',
  Notes       VARCHAR(128) NULL,
  PRIMARY KEY (TableSource, DbcName)
) ENGINE=InnoDB;

INSERT INTO client_dbc_export_map VALUES
  ('broadcast_text',        'BroadcastText',   '/Game/Data/Databases/DT_BroadcastText',   'диалоги мобов/NPC: текст по text_id'),
  ('creature_template',     'Creature',        '/Game/Data/Databases/DT_Creature',        'model, level, faction, scripts'),
  ('gossip_menu',           'GossipMenu',      '/Game/Data/Databases/DT_GossipMenu',      'заголовок меню (greeting)'),
  ('gossip_menu_option',    'GossipOption',    '/Game/Data/Databases/DT_GossipOption',    'иконки/тексты опций (фильтр — на сервере!)'),
  ('quest_template',        'Quest',           '/Game/Data/Databases/DT_Quest',           'журнал квестов, POI-маркеры'),
  ('item_template',         'Item',            '/Game/Data/Databases/DT_Item',            'тултипы, иконки, цвета качества'),
  ('vendor_rec',            'VendorItem',      '/Game/Data/Databases/DT_Vendor',          'витрины торговых NPC'),
  ('quest_chain',           'QuestChain',      '/Game/Data/Databases/DT_QuestChain',      'стрелки «продолжить у...» в журнале');

-- Подсказки POI для клиента (куда идти): наследует формат TRC quest_poi
CREATE TABLE IF NOT EXISTS quest_poi (
  QuestId SMALLINT UNSIGNED NOT NULL,
  MapId   SMALLINT UNSIGNED NOT NULL,
  X INT NOT NULL, Y INT NOT NULL, FloorId TINYINT NOT NULL DEFAULT 0,
  IconType TINYINT NOT NULL DEFAULT 1 COMMENT '1 kill 2 talk 3 gather',
  Name TEXT NULL, PRIMARY KEY (QuestId, MapId, X, Y)
) ENGINE=InnoDB;

INSERT INTO quest_poi VALUES
  (1001, 1100, 65536*12, 65536*9, 0, 2, 'Командор Веларион'),
  (1002, 1201, 65536*20, 65536*15, 0, 1, 'Слизни устья'),
  (1004, 1201, 65536*20, 65536*15, 0, 1, 'Пепельный Карьер'),
  (1005, 1100, 65536*30, 65536*22, 0, 1, 'Устье Истечения'),
  (1006, 1100, 65536*40, 65536*25, 0, 1, 'Старый мост'),
  (1007, 1400, 65536*15, 65536*12, 0, 1, 'Мост Гримма'),
  (1003, 1400, 65536*5,  65536*5,  0, 2, 'Контрабандист Гримм');
