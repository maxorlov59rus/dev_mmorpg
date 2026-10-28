-- =====================================================================
-- «Чёрная Материя» — StarterQuests.sql (dm_world, контент v2)
-- Стартовые квесты фракций + путь Ренегата.
--
-- Соглашения:
--  * RewRepValue в RAW Q16 единицах ReputationMgr (1 очко = 16 ед).
--    ТЗ «+250 репутации» => 250*16 = 4000.
--  * Faction ids: 1 = Стражи Света (Альянс Света), 2 = Падшие (Орден Пепла),
--                 4 = служебная фракция изгоев.
--  * Id-диапазоны как в TRC: quests 1000+, npcs 90000+, items 5000+.
-- =====================================================================

USE dm_world;

-- ---------- Предметы стартовых сетов (награды квестов) ----------
-- Альянс Света: Cloth (маги/жрецы) + Plate (воины) — Material по TRC-семантике.
INSERT INTO item_template
  (entry, class, subclass, name, displayid, Quality, ItemLevel, RequiredLevel,
   AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank, Material)
VALUES
  (5101, 4, 1, 'Риза Послушника Рассвета',      25101, 1, 1, 1, -1, -1, 1 /*BoP*/, 1, 0, 1 /*cloth*/),
  (5102, 4, 3, 'Наплечники Новобранца Стали',   25102, 1, 1, 1,  1 /*Воин*/, -1, 1, 1, 0, 4 /*plate*/),
  (5103, 2, 0, 'Милосердный Жезл Аcolита',      25103, 2, 3, 1,  2|3 /*Жрец,Маг*/, -1, 1, 1, 0, 0),
  -- Орден Пепла: Leather + Mail
  (5201, 4, 2, 'Пояс Шипов Кожевника',          25201, 1, 1, 1,  4|5 /*Плут,Охотник*/, -1, 1, 2, 0, 2 /*leather*/),
  (5202, 4, 3, 'Кольчуга Пепельного Рекрута',   25202, 1, 1, 1,  1, -1, 1, 2, 0, 3 /*mail*/),
  (5203, 2, 0, 'Кинжал «Уголёк»',               25203, 2, 3, 1,  4, -1, 1, 2, 0, 0),
  -- Квестовые предметы
  (5301, 1, 0, 'Донесение Командору',           13301, 3, 1, 1, -1, -1, 3 /*quest*/, 1, 0, 0),
  -- Ресурс из ТЗ (артефакт 4): Этерниум, DropFaction=2 (см. mining_node_def)
  (5001, 6, 0, 'Этерниум (самородок)',          15001, 1, 1, 1, -1, -1, 0, 0, 0, 0);

-- ---------- NPC для стартовых линий ----------
-- faction_A/faction_H — альянсовые маски; unit_flags2 бит21 у стражей городов.
INSERT INTO creature_template (entry, name, subname, min_level, max_level,
   faction_A, faction_H, unit_flags, unit_flags2, lootid, script_name) VALUES
  (90001, 'Командор Веларион', 'Разведка Ордена Рассвета', 5, 5, 0x0001, 0, 0, 0, 0, 'npc_starter_light'),
  (90002, 'Хранитель Пепла Морвен', 'Испытание Ордена',    5, 5, 0, 0x0002, 0, 0, 0, 'npc_starter_fallen'),
  (90003, 'Слизень Истечения', '', 3, 4, 0, 0x0002, 0x0000, 0, 300, ''),        -- цель q_1002 (моб)
  (90004, 'Контрабандист Гримм', 'Только для изгоев',     20, 20, 0, 0, 0, 0, 0, 'npc_black_market'),
  (90005, 'Страж городских врат', 'Застава Рассвета',      30, 30, 0x0001, 0, 0, 2097152 /*unit_flags2 bit21 AttackRenegade*/, 0, '');

-- ---------- Квесты ----------
-- q_1001 «Донесение командору»: talk-goal (RequiredNpcOrGo1 = npc guid entry).
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RewItemId1, RewItemCount1, RewItemId2, RewItemCount2,
   RequiredNpcOrGo1, RequiredNpcOrGoCount1, MinLevel, ZoneId)
VALUES (1001, 0, 0, 16 /*SpecialFlags: bit4 autostart*/,
  'Донесение командору',
  'Граница дрожит. Отнеси запечатанный свиток Командору Велариону в лагерь Рассвета — он решает, куда направить послушников.',
  'Поговорите с Командором Веларионом',
  'Хорошо, что ты выжил. Чёрная Материя не прощает медлительных. Держи снаряжение — оно твоё по праву клятвы.',
  'Веларион вскрывает печать и читает...',
  1000,                                  -- 1 gold copper-analog
  1, 4000 /*+250 очков Стражам Света*/,  2, -800 /*-50 Падшим*/,
  5101, 1, 5102, 1,                       -- cloth + plate сет
  90001, 1, 1, 1100);

-- q_1002 «Испытание тьмой»: kill 5x Слизень Истечения (90003).
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RewItemId1, RewItemCount1, RewItemId2, RewItemCount2,
   RequiredCreatureKill1, RequiredCreatureKillCount1, MinLevel, ZoneId)
VALUES (1002, 0, 0, 16,
  'Испытание тьмой',
  'Покажи, что Пепел не сжёг твой разум. У устья Истечения кишат слизни — они питаются осколками Чёрной Материи. Убей пятерых и принеси их ядра.',
  'Убейте: Слизень Истечения (5)',
  'Ты испачкался в их слизи, но не в их воле. Это различие важно. Носи эти доспехи достойно.',
  'Морвен взвешивает ядра на ладони и кивает.',
  1000,
  1, -800,                               -- -50 Стражам Света
  2, 4000 /*+250 Падшим*/,
  5201, 1, 5202, 1,                        -- leather + mail сет
  90003, 5, 1, 1200);

-- q_1003 «Изгой» — СКРЫТЫЙ, авто-выдача при переходе порога ренегата.
-- Выдаётся кодом: ReputationMgr::OnBecomeRenegate -> QuestMgr::AddQuest(1003).
-- StartEvent='on_renegade_gained' — маркер для миграций/хотов, чтобы админы
-- видели источник выдачи; IsHidden=1 — не светится в journal до события.
INSERT INTO quest_template (ID, QuestType, Flags, SpecialFlags, Title, Details, Objectives,
   OfferRewardText, CompleteText, Reward,
   RewRepFaction1, RewRepValue1, RewRepFaction2, RewRepValue2,
   RequiredNpcOrGo1, RequiredNpcOrGoCount1, StartEvent, IsHidden, MinLevel, ZoneId)
VALUES (1003, 0, 1 /*Flags: bit0 hidden*/, 16 /*SpecialFlags: bit4 autostart*/,
  'Изгой',
  'Ни Знамя Рассвета, ни Пепел не примут тебя. Но Порог всегда торгует с отчаявшимися. Найди контрабандиста Гримма в нейтральных землях — за мостом, где нет флагов.',
  'Найдите Контрабандиста Гримма',
  'Изгой, значит. Хорошо — с тобой удобно работать. С этого часа Чёрный рынок открыт для тебя. Не благодари.',
  'Гримм поворачивает ключ в потайной двери.',
  0,
  1, 0, 2, 0,                              -- rep НЕ двигается (награда — доступ)
  90004, 1,
  'on_renegade_gained', 1, 10, 1400);

-- Награда q_1003 = флаг доступа к VENDOR_BLACK_MARKET: кодируется через
-- reward-предмет "Пропуск на Чёрный рынок" (FactionRelated=4) — его наличие
-- проверяет SmartAI::GOSSIP_ACTION_RENEGADE_BLACKMARKET.
INSERT INTO item_template (entry, class, subclass, name, displayid, Quality, ItemLevel,
  RequiredLevel, AllowableClass, AllowableRace, Bonding, FactionRelated, FactionReqRank)
VALUES (5401, 1, 0, 'Пропуск на Чёрный рынок', 15401, 4 /*epic*/, 1, 1, -1, -1, 1 /*BoP*/, 4, 0);
UPDATE quest_template SET RewItemId1 = 5401, RewItemCount1 = 1 WHERE ID = 1003;

-- ---------- Привязка стартовых квестов к NPC-старторам (quest_relations) ----------
-- Аналог TRC creature_queststarter: autostart фильтруется по ReqMinFactionValue.
CREATE TABLE IF NOT EXISTS creature_quest_relation (
  creature_entry MEDIUMINT UNSIGNED NOT NULL,
  quest_id       MEDIUMINT UNSIGNED NOT NULL,
  role           ENUM('start','end') NOT NULL,
  PRIMARY KEY (creature_entry, quest_id, role)
) ENGINE=InnoDB;

INSERT INTO creature_quest_relation VALUES
  (90001, 1001, 'start'), (90001, 1001, 'end'),
  (90002, 1002, 'start'), (90002, 1002, 'end'),
  (90004, 1003, 'end');

-- ---------- Стартовые экипировочные наборы (dm_characters.starting_outfit) ----------
USE dm_characters;
INSERT INTO starting_outfit (faction, class, race, items_json) VALUES
  (1, 1, 1, '[{"entry":5102,"count":1},{"entry":5001,"count":2}]'),   -- Воин Альянса: plate
  (1, 2, 1, '[{"entry":5101,"count":1},{"entry":5103,"count":1}]'),   -- Жрец: cloth + жезл
  (1, 3, 1, '[{"entry":5101,"count":1},{"entry":5103,"count":1}]'),   -- Маг: cloth + жезл
  (2, 4, 2, '[{"entry":5201,"count":1},{"entry":5203,"count":1}]'),   -- Плут Пепла: leather + кинжал
  (2, 5, 2, '[{"entry":5201,"count":1}]'),                            -- Охотник: leather
  (2, 1, 2, '[{"entry":5202,"count":1}]');                            -- Воин Пепла: mail
