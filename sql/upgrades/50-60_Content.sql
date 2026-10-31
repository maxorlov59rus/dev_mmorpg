-- ============================================================================
-- 50-60_Content.sql — Контент для уровней 50-60
-- База: dm_world
-- ============================================================================

-- === Квесты уровня 50-60 ===

INSERT IGNORE INTO quest_def (id, title_ru, title_en, description_ru, description_en, level_min, level_max, quest_type, quest_category, objectives, reward_type, reward_value, time_limit, is_repeatable, faction_id) VALUES
(101, 'Путь на Вершину', 'Path to the Summit', 'Покорите 10 горных пиков', 'Climb 10 mountain peaks', 50, 55, 'kill_monsters', 'exploration', 'defend_pass, slay_10_mountain_trolls, retrieve_ancient_artifact', 'gold', '200', 0, 1),
(102, 'Забытыйхранилище', 'Forgotten Vault', 'Найдите древнийartifact в заброшенном храме', 'Find ancient artifact in abandoned temple', 55, 60, 'collect_items', 'dungeon', 'collect_5_artifact_fragments, defeat_ancient_guardian, retrieve_lost_knowledge', 'spirit', '150', 0, 1);

-- === НПЦы уровня 50-60 ===

INSERT IGNORE INTO npc_def (id, name_ru, name_en, title_ru, title_en, npc_type, rank, faction_id, city_id, gender, race, description_ru, description_en, is_quest_giver, is_trainer, is_special_interact) VALUES
(201, 'Губитель Гор', 'Mountain Slayer', 'Хранитель гор', 'Mountain Guardian', 0, 2, 1, 101, 0, 'human', 'Ветерана гор, который охраняет пути на вершины', 'Master of the Peaks', 1, 0, 1),
(202, 'Архивариух', 'Forgotten Archivist', 'Хранитель следов', 'Forgotten Keeper', 0, 2, 2, 102, 1, 'elf', 'Стараяエルフ, которая хранит секреты древнего храма', 'Keeper of Secrets', 1, 0, 1);

-- === Мобы уровня 50-60 ===

INSERT IGNORE INTO monster_def (id, name_ru, name_en, level, faction_id, hp, mana, exp_reward, gold_reward, attack_power, defense, quest_id) VALUES
(301, 'Горный титаник', 'Mountain Titanic', 52, 1, 5000, 2000, 300, 500, 200, 150, 101),
(302, 'Древний страж', 'Ancient Guardian', 57, 2, 8000, 3000, 400, 750, 250, 200, 102);

-- === Лут уровня 50-60 ===

INSERT IGNORE INTO loot_table (id, item_id, monster_id, chance, quantity, description_ru, description_en) VALUES
(201, 50001, 302, 15, 1, 'Древнийartifact', 'Ancient Artifact'),
(202, 40005, 301, 20, 1, 'Горная гарь', 'Mountain Halberd');

-- === Ресурсы уровня 50-60 ===

INSERT IGNORE INTO resource_table (id, name_ru, name_en, type, stack_size, respawn_time, gathering_level_min, description_ru, description_en) VALUES
(301, 'Горной камень', 'Mountain Stone', 'crafting', 999, 300, 50, 'Материал для горных зачарований', 'Material for mountain enchantments'),
(302, 'Древняя чешуя', 'Ancient Scale', 'crafting', 999, 600, 55, 'Материал для эпических зачарований', 'Material for epic enchantments');

-- === Диалоги уровня 50-60 ===

INSERT IGNORE INTO dialogue_def (id, npc_id, dialogue_type, text_ru, text_en, emotion, voice_id, priority) VALUES
(501, 201, 'quest_start', 'Уважаемый искатель приключений, горы хранят древние секреты. Я, Губитель Гор, уверен, вы справитесь с преодолением десяти пиков. Примите этот вызов!', 'Venerable adventurer, the mountains hold ancient secrets. I, the Mountain Slayer, am certain you will succeed in overcoming the ten peaks. Accept this challenge!', 'friendly', 101, 1),
(502, 202, 'quest_start', 'Добро пожаловать в заброшенный храм. Здесь хранятся древниеartifact, которые способны изменить Schicksale entire regio. Я, Архивариух, назначаю вас на миссию изыскания artifactа. Готовы ли вы к исследованию?', 'Welcome to the forgotten temple. Here ancient artifacts are stored that can change the destiny of entire regio. I, the Forgotten Archivist, assign you a mission of artifact retrieval. Are you ready for exploration?', 'mysterious', 202, 2);

-- === Госсипы уровня 50-60 ===

INSERT IGNORE INTO gossip_def (id, entry, text_ru, text_en, sound_delay) VALUES
(601, 201, 'Слушали вы, как Губитель Гор требует доказательств сил? Убить 10 горных титанов и получить древний artifact!', 'Heard the Mountain Slayer demands proof of strength? Slay 10 Mountain Titans and get an Ancient Artifact!', 3000);
