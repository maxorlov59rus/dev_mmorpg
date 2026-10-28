-- =============================================================================
-- «Чёрная Материя» — dm-server / scripts/lua/npc/npc_quest_giver_10.lua
-- NPC-наставник 10 уровня: «Командир Ополчения» (entry 90021, Орден Рассвета)
-- и «Смотрящая за Пеплом» (entry 90022, Круг Поглощения).
-- SQL-привязка: sql/upgrades/Insert_Quests_6_to_10.sql (creature_template +
-- gossip_menu 1104xx, smart_scripts source_entry).
--
-- Поведение (аналог TRC smart_scripts Event=ACCEPTED_GOSSIP/SELECT_GOSSIP):
--   * PlayerLevel < 10  -> текст «Возвращайся, когда станешь сильнее»,
--                          квестовая опция скрыта (condition_id гейт на сервере).
--   * PlayerLevel >= 10 -> предлагает q_1104 «Путь избранного» (иконка quest!),
--                          после сдачи q_1104 — выдаёт «Рупор призыва» (5120)
--                          и открывает опцию LFG-очереди.
--
-- API-контракт см. quest_1004_spec_choice.lua (шапка). Дополнительно:
--   player:AddItem(item_entry, count)
--   npc:GetEntry() / GetName()
-- =============================================================================

local NPC_MILITIA_ALLIANCE = 90021 -- Командир Ополчения (фракция 1)
local NPC_MILITIA_ASHEN    = 90022 -- Смотрящая за Пеплом (фракция 2)

local ITEM_LFG_HORN = 5120 -- «Рупор призыва» (BoP, consumes -> вызывает CMSG_JOIN_LFG UI)
local Q_PATH_CHOSEN = 1104
local Q_TRIAL_BY_FIRE = 1105

-- ---------------------------------------------------------------------------
-- Gossip hello: динамический текст по уровню (ядро шлёт SMSG_GOSSIP_SHOW с
-- уже отфильтрованными опциями; здесь только выбор greeting text id)
-- ---------------------------------------------------------------------------
local function OnGossipHello(event, player, creature)
    if player:GetLevel() < 10 then
        -- «Возвращайся, когда станешь сильнее» (broadcast_text 90030)
        player:SendGossipText(creature, 90030)
        return false -- не показываем меню квеста
    end

    local spec = player:GetActiveSpec()
    if not spec or spec == 0 then
        player:SendGossipMenu(creature, 110400 + player:GetClass()) -- выбор спеки
    else
        player:SendGossipMenu(creature, 110500) -- LFG-ветка: рупор + «Проверка боем»
    end
    return true
end

RegisterCreatureGossipEvent(NPC_MILITIA_ALLIANCE, 1, OnGossipHello)
RegisterCreatureGossipEvent(NPC_MILITIA_ASHEN, 1, OnGossipHello)

-- ---------------------------------------------------------------------------
-- Gossip select: option_id 1 = «Дать квест пути», 2 = «Выдать Рупор призыва»
-- ---------------------------------------------------------------------------
local function OnGossipSelect(event, player, creature, sender, index)
    -- Серверный guard: уровень проверяем ПОВТОРНО (клиент может прислать любой
    -- index — паттерн packet-replay из QuestGossipHandlers.cpp).
    if player:GetLevel() < 10 then
        player:SendGossipText(creature, 90030)
        player:CloseGossip()
        return
    end

    if index == 1 then
        if player:GetQuestStatus(Q_PATH_CHOSEN) == 1 then -- IN_PROGRESS
            player:SendGossipText(creature, 90031)       -- «Ты уже в пути...»
        elseif not player:HasQuest(Q_PATH_CHOSEN) then
            player:AddQuest(Q_PATH_CHOSEN)               -- accept через ядро (Lua->C++)
            player:SendGossipText(creature, 90032)       -- «Звёзды смотрят на тебя.»
        end
        player:CloseGossip()
    elseif index == 2 then
        -- Выдача LFG Token: только после выбора спеки и НЕ повторно.
        local spec = player:GetActiveSpec()
        if spec and spec ~= 0 and not player:HasItem(ITEM_LFG_HORN) then
            player:AddItem(ITEM_LFG_HORN, 1)
            if not player:HasQuest(Q_TRIAL_BY_FIRE) then
                player:AddQuest(Q_TRIAL_BY_FIRE)
            end
            player:SendGossipText(creature, 90033) -- «Протруби в него у костра...»
        else
            player:SendGossipText(creature, 90034) -- отказ/уже есть
        end
        player:CloseGossip()
    end
end

RegisterCreatureGossipEvent(NPC_MILITIA_ALLIANCE, 2, OnGossipSelect)
RegisterCreatureGossipEvent(NPC_MILITIA_ASHEN, 2, OnGossipSelect)
