-- =====================================================================
-- «Чёрная Материя» — NPC_Config.sql (dm_world, данные v2)
-- Конфигурация gossip-NPC с ветками по репутации, фракционные мобы,
-- лут-правила зон и руда «Этерниум» со штрафом для ренегатов.
-- =====================================================================

USE dm_world;

-- ---------------------------------------------------------------------
-- 1) GOSSIP: статические меню + condition_id (фильтр показа опций).
--    Runtime-guard той же логики — SmartAI::GossipOptionSatisfiesCondition
--    (защита от packet-replay на опции, которые не показывались).
--    ActionType (поле Action): 0=talk-close, 1=trade(vendor), 2=quest-menu,
--                              3=rep-summary, 4=black-market (только изгои).
-- ---------------------------------------------------------------------
INSERT INTO gossip_menu (ID, TextID, ScriptName) VALUES
  (5001, 90001, 'gossip_starter_light'),   -- Командор Веларион
  (5002, 90002, 'gossip_starter_fallen'),  -- Хранитель Морвен
  (5003, 90004, 'gossip_black_market');    -- Контрабандист Гримм

INSERT INTO gossip_menu_option
  (MenuID, ID, OptionIcon, OptionText, OptionType, Action, condition_id)
VALUES
  -- Командор: базовые ветки доступны всем не-изгоям
  (5001, 1, 0, 'Докладываю, командор.',            1 /*quest*/, 2 /*quest-menu*/, 3 /*not_renegade*/),
  (5001, 2, 3, 'Что у вас в лавке?',               2 /*trade*/,  1 /*vendor*/,     3),
  -- Торговля выше базового уровня — только при >= FRIENDLY (condition id=1)
  (5001, 3, 3, 'Покажите тайный ассортимент.',     2, 1, 1 /*rep_rank_min f1 FRIENDLY*/),
  -- Сводка репутации
  (5001, 4, 7, 'Как относится Орден к моим делам?', 0, 3 /*rep-summary*/, 3),
  -- Изгой: единственная ветка у стражей/городских NPC — угроза (текст)
  (5001, 5, 0, 'Я прошёл мимо ваших знамён...',    0, 0, 2 /*is_renegade*/),

  -- Морвен (Орден Пепла) — зеркально
  (5002, 1, 0, 'Пепел помнит меня.',               1, 2, 3),
  (5002, 2, 3, 'Снарядите меня.',                  2, 1, 2 /*rep_rank_min f2 FRIENDLY*/),
  (5002, 3, 7, 'Какова моя цена для Ордена?',      0, 3, 3),

  -- Гримм: Чёрный рынок — ТОЛЬКО ренегаты (condition id=3 is_renegade)
  (5003, 1, 3, 'Мне нужен товар без флагов.',      2, 4 /*black-market*/, 3),
  (5003, 2, 0, 'Слухи за осколок.',                0, 0, 3);

-- Привязка меню к NPC (creature_template.gossip_menu_id добавим ALTER'ом ниже,
-- как делают TRC-форки; храня guid'ы отдельно — как creature_quest_relation).
ALTER TABLE creature_template
  ADD COLUMN gossip_menu_id MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 AFTER script_name;

UPDATE creature_template SET gossip_menu_id = 5001 WHERE entry = 90001;
UPDATE creature_template SET gossip_menu_id = 5002 WHERE entry = 90002;
UPDATE creature_template SET gossip_menu_id = 5003 WHERE entry = 90004;

-- Тексты агрессии стражей против ренегатов (TEXT_GUARD_RENEGADE_CHALLENGE=90100,
-- TEXT_HOSTILE_TAUNT[1]=90101, TEXT_HOSTILE_TAUNT[2]=90102 — см. SmartAI.cpp):
CREATE TABLE IF NOT EXISTS broadcast_text (
  ID MEDIUMINT UNSIGNED NOT NULL PRIMARY KEY,
  Text0 TEXT NOT NULL, LanguageID TINYINT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB;
INSERT INTO broadcast_text VALUES
  (90100, 'Изгой! Здесь нет места для тех, кто предал оба знамени. Стоять!', 0),
  (90101, 'Тварь Пепла... даже мёртвый Аcolит лучше тебя. Беги, пока цел.', 0),
  (90102, 'Свет отвернулся от тебя, предатель. Тьма тоже.', 0);

-- ---------------------------------------------------------------------
-- 2) МОБЫ: фракционная принадлежность (ТЗ: faction = 1650 для Ордена Пепла).
--    Диапазон 1650..1699 зарезервирован под «Падших», 1600..1649 — «Стражи».
--    Совпадает с маской faction_A/faction_H шаблонов выше (0x0001/0x0002).
-- ---------------------------------------------------------------------
INSERT INTO creature_template (entry, name, subname, min_level, max_level,
   faction_A, faction_H, unit_flags, unit_flags2, lootid, script_name) VALUES
  (16501, 'Пепельный берсерк',        'Орден Пепла', 12, 13, 0,    1650, 0, 0, 301, ''),
  (16502, 'Жрица Тлеющего Алтаря',    'Орден Пепла', 14, 14, 0,    1650, 0, 0, 302, ''),
  (16001, 'Лазутчик Рассвета',        'Стражи Света', 12, 13, 1600, 0,   0, 0, 301, ''),
  (16002, 'Световой голем-страж',     'Стражи Света', 15, 15, 1600, 0,   0, 2097152, 303, ''); -- unit_flags2 bit21: атакует изгоев

-- Настройка агрессивности к ренегатам массово для городских стражей:
UPDATE creature_template
  SET unit_flags2 = unit_flags2 | 2097152 /* bit21 AttackRenegade */
  WHERE entry IN (90005, 16002) OR script_name LIKE 'npc_guard_%';

-- ---------------------------------------------------------------------
-- 3) ЛУТ: reference-таблицы фракционных зон (зона 1200 = фронт «Пепелище»).
--    LootMgr резолвит Entry через zone_loot_rule по frontline_control.owner_faction:
--    владелец=2 (Пепел) -> группа 310 (осколки Умбры), владелец=1 -> 311 (Люмен),
--    ничейно -> 300 (базовый мусор). Дроп меняется динамически без патчей.
-- ---------------------------------------------------------------------
INSERT INTO reference_loot_template (Entry, Item, Chance, MinCount, MaxCount, Comment) VALUES
  (300, 5001, 10.00, 1, 2, 'Этерниум — базовый дроп (ничейная зона)'),
  (300, 5203,  1.50, 1, 1, 'Кинжал Уголёк — редкость'),
  (310, 5001, 25.00, 2, 4, 'Этерниум под контролем Пепла: чаще'),
  (310, 5202,  4.00, 1, 1, 'Кольчуга рекрута'),
  (311, 5001, 25.00, 2, 4, 'Этерниум под контролем Стражей'),
  (311, 5102,  4.00, 1, 1, 'Наплечники новобранца');

INSERT INTO zone_loot_rule (zone_id, owner_faction, loot_group) VALUES
  (1200, 0, 300), (1200, 1, 311), (1200, 2, 310),
  (1100, 0, 300), (1100, 1, 311), (1100, 2, 310);

-- Привязка групп к мобам (lootid уже выставлен; 301/302 — общие, 303 — голем):
INSERT INTO reference_loot_template (Entry, Item, Chance, Comment) VALUES
  (301, 5001, 15.00, 'Фракционный моб — этерниум'),
  (302, 5001, 20.00, 'Жрица — этерниум x2 шанс'),
  (303, 5001, 30.00, 'Голем — жирный дроп');

-- ---------------------------------------------------------------------
-- 4) РЕСУРСЫ: Этерниум (item 5001), DropFaction=2 => ренегаты добывают на 50%.
--    Штраф применяет Player::UpdateSkills/Mining, читая mining_node_def:
--    speed_mult = (renegade && !(mask & RENEGADE_BIT)) ? pct/100 : 1.0
-- ---------------------------------------------------------------------
-- node_entry 7100 = gameobject "Жила Этерниума"; mask 0b110: фракции 1 и 2
-- имеют бонус добычи, у ренегата (бит 4) маски нет => применяется штрафной
-- множитель renegade_speed_pct_x100 = 50 (копает в 2 раза медленнее).
INSERT INTO mining_node_def (node_entry, item_entry, drop_faction_mask,
   renegade_speed_pct_x100, min_skill, respawn_ms) VALUES
  (7100, 5001, 0b00000110, 50, 150, 300000);

-- Игровые узлы жил в мире (spawn-позиции — аналог TRC gameobject_spawn):
CREATE TABLE IF NOT EXISTS gameobject_spawn (
  guid SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  entry MEDIUMINT UNSIGNED NOT NULL, map_id SMALLINT UNSIGNED NOT NULL,
  zone_id SMALLINT UNSIGNED NOT NULL, pos_x INT NOT NULL, pos_y INT NOT NULL, pos_z INT NOT NULL,
  orientation INT NOT NULL DEFAULT 0, phase_mask INT UNSIGNED NOT NULL DEFAULT 0xFFFFFFFF,
  KEY idx_zone (zone_id, entry)
) ENGINE=InnoDB;
INSERT INTO gameobject_spawn (entry, map_id, zone_id, pos_x, pos_y, pos_z) VALUES
  (7100, 1200, 1200, 458752, -102400, 30720),
  (7100, 1200, 1200, 461312, -101888, 31232),
  (7100, 1100, 1100, -350208, 220160, 28672);
