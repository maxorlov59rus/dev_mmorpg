-- =============================================================================
-- «Чёрная Материя» — dm-server / scripts/lua/quests/quest_1004_spec_choice.lua
-- Квест 1104 «Путь избранного» (SQL: sql/upgrades/Insert_Quests_6_to_10.sql).
--
-- Ядро смены специализации на 10 уровне. Скриптовый слой Eluna-style
-- (docs/02 §Scripts): хуки регистрируются в HookMgr, серверное ядро вызывает
-- их из QuestSystem_Level10.cpp / QuestGossipHandlers.cpp.
--
-- Контракт API (инжектится ScriptEngine при старте worldserver):
--   RegisterQuestEvent(quest_id, event_id, fn)      -- quest.on_accept/on_complete
--   RegisterPlayerEvent(event_id, fn)               -- см. PLAYER_EVENT_* ниже
--   player:GetLevel() / GetClass() / GetRace() / GetGUID()
--   player:GetActiveSpec() -> spec_id | nil
--   player:GrantSpecAbilities(spec_id)              -- мост SpecQuestMgr::GrantSpecAbilities
--   player:ChooseSpec(spec_id)                      -- полный путь CMSG_CHOOSE_SPEC
--   player:AddQuest(quest_id) / CompleteQuest(quest_id) / RewardQuest(quest_id)
--   player:LearnSpell(spell_id)
--   player:SendGossipMenu(npc, menu_id)             -- открыть окно выбора
--   dm.log(level, msg)                              -- world-log
--
-- Event IDs (зеркало enum QuestEvents в ScriptEngine.h):
local QUEST_ON_ACCEPT   = 1
local QUEST_ON_COMPLETE = 2
local QUEST_ON_ABANDON  = 3

local PLAYER_EVENT_ON_LOGIN         = 3 -- после SMSG_LOGIN_COMPLETE
local PLAYER_EVENT_ON_LEVEL_CHANGED = 7 -- (player, oldLevel, newLevel)

-- ID квестов цепочки 6–10 (контракт с QuestSystem_Level10.h!)
local Q_PATH_CHOSEN   = 1104 -- «Путь избранного» (выбор спеки)
local Q_TRIAL_BY_FIRE = 1105 -- «Проверка боем» (обучающий инстанс через LFG)

-- Специализации по классам (контракт class_specs / SpecId из PlayerClassMgr.h)
local SPECS_BY_CLASS = {
    [1] = { -- Воин
        { id = 1, name = 'Осадный',  role = 'DPS' },
        { id = 2, name = 'Авангард', role = 'TANK' },
        { id = 3, name = 'Дуэлянт',  role = 'DPS' },
    },
    [3] = { -- Маг
        { id = 4, name = 'Мороз',     role = 'DPS' },
        { id = 5, name = 'Истечения', role = 'DPS' },
        { id = 6, name = 'Свет',      role = 'HEALER' },
    },
    [4] = { -- Разбойник
        { id = 7, name = 'Тень',      role = 'DPS' },
        { id = 8, name = 'Послушник', role = 'DPS' },
        { id = 9, name = 'Порожник',  role = 'DPS' },
    },
}

-- -----------------------------------------------------------------------------
-- OnAccept: показать окно выбора специализации (gossip-меню 1104xx по классу)
-- -----------------------------------------------------------------------------
RegisterQuestEvent(Q_PATH_CHOSEN, QUEST_ON_ACCEPT, function(event, player, quest, npc)
    local cls = player:GetClass()
    local specs = SPECS_BY_CLASS[cls]
    if not specs then
        dm.log('warn', string.format('quest_1004: нет спек для класса %s (персонаж guid=%s)',
                                     tostring(cls), tostring(player:GetGUID())))
        return
    end

    -- Готовим gossip-меню: option_id = индекс спекы (1-based). Сервер отдаёт
    -- список; клиент UI_SpecSelection открывает модалку по SMSG_GOSSIP_REPLY
    -- action=6 (SPEC_SELECTION) либо напрямую по SMSG_SPEC_SELECTION_OPEN (0x2023).
    local menuId = 110400 + cls
    player:SendGossipMenu(npc, menuId)
    dm.log('debug', string.format('quest_1004: открыто меню выбора спеки %d (класс %d, %d опций)',
                                  menuId, cls, #specs))
end)

-- -----------------------------------------------------------------------------
-- OnComplete: применяем выбранную спеку -> способности -> выдаём q_1105 (LFG-обучение)
-- ВАЖНО: сам выбор спеки делает CMSG_CHOOSE_SPEC (серверный guard!), сюда Lua
-- приходит только ПОСЛЕ успешного ChooseSpec — quest progress flag ставит ядро.
-- -----------------------------------------------------------------------------
RegisterQuestEvent(Q_PATH_CHOSEN, QUEST_ON_COMPLETE, function(event, player, quest, npc)
    local spec = player:GetActiveSpec()
    if not spec or spec == 0 then
        -- Защита: сдача без выбора невозможна (ядро не ставит goal completion),
        -- но на случай packet-replay — блокируем и закрываем диалог.
        dm.log('warn', 'quest_1004: complete без активной спеки — отклонено')
        return false -- false = запретить сдачу
    end

    -- Выдача способностей специализации (idempotent внутри C++-моста).
    local learned = player:GrantSpecAbilities(spec) or {}
    dm.log('info', string.format('quest_1004: игрок guid=%s выбрал спеку %d, выучено спеллов: %d',
                                 tostring(player:GetGUID()), spec, #learned))

    -- Золотая подсветка новых способностей на action bar клиента делает сам
    -- клиент по SMSG_LEARNED_SPELLS_BATCH (source=spec) — здесь ничего шить не надо.

    -- Цепочка: выдаём обучающий инстанс-квест через LFG («Проверка боем»).
    if player:GetLevel() >= 10 then
        player:AddQuest(Q_TRIAL_BY_FIRE)
    end
    return true
end)

-- Abandon — не разрешаем бросать квест выбора спеки после подтверждения
-- (спека уже применена; возврат — только через тренера, M2).
RegisterQuestEvent(Q_PATH_CHOSEN, QUEST_ON_ABANDON, function(event, player, quest)
    if player:GetActiveSpec() and player:GetActiveSpec() ~= 0 then
        dm.log('info', 'quest_1004: попытка abandon после выбора спеки — запрещена')
        return false
    end
    return true
end)

-- -----------------------------------------------------------------------------
-- Страховка уровня: если игрок догнал 10 уровень без автовыдачи (апгрейд персов
-- со старого контента) — выдаём квест при пересечении 10-го уровня.
-- -----------------------------------------------------------------------------
RegisterPlayerEvent(PLAYER_EVENT_ON_LEVEL_CHANGED, function(event, player, oldLevel, newLevel)
    if newLevel >= 10 and oldLevel < 10 then
        if (not player:GetActiveSpec()) or player:GetActiveSpec() == 0 then
            player:AddQuest(Q_PATH_CHOSEN) -- дубликат-защита на стороне C++ AddQuest
        end
    end
end)
