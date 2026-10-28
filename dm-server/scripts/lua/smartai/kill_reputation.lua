-- =====================================================================
-- «Чёрная Материя» — SmartAI Lua: репутация за убийство моба (ТЗ артефакт 3)
-- Аналог TrinityCore smart_scripts, но скриптуется на Lua (HookMgr/Eluna-style).
--
-- Контракт хука (реализован в dm-server/src/game/AI/SmartAI.cpp::OnKillCredit):
--   HookMgr.CallVoid("smartai.on_kill_credit", killer, killed)
-- Параметры:
--   player  : userdata (Player*), методы GetGUID/GetFaction/IsRenegate/...
--   creature: userdata (Creature*), методы GetEntry()/GetName()
-- API начисления доступен глобалом `dm` (инжектится ScriptEngine при старте):
--   dm.modify_reputation(player, faction_id, amount_q16, source_string)
-- amount в RAW Q16: 1 очко = 16 ед => +250 очков = 4000.
-- =====================================================================

local REP = {
    ORDER_OF_LIGHT = 1,   -- «Стражи Света» / Орден Рассвета
    FALLEN         = 2,   -- «Падшие» / Круг Поглощения
    THRESHOLD      = 3,   -- Гильдия Порога
    RENEGATE       = 4,   -- служебная фракция изгоев
}

-- Таблица «моб -> награда/штраф». Правится контент-дизайнером без рекомпиляции;
-- позже мигрирует в hotfix-таблицу smart_scripts (dm_world), это interim-слой.
local KILL_REP_TABLE = {
    -- Слизни Истечения (цель q_1002): +15 очков Стражам Света, -8 Падшим.
    [90003] = { reward = { f = REP.ORDER_OF_LIGHT, v = 240 },    -- 15*16
                penalty = { f = REP.FALLEN,        v = -128 } }, -- 8*16
    -- Пепельный берсерк: зеркально (фарм «своими» против «своих» мобов)
    [16501] = { reward = { f = REP.FALLEN,          v = 160 },
                penalty = { f = REP.ORDER_OF_LIGHT, v = -96 } },
    [16502] = { reward = { f = REP.FALLEN,          v = 240 },
                penalty = { f = REP.ORDER_OF_LIGHT, v = -96 } },
    -- Лазутчик Рассвета — для фармеров Ордена Пепла
    [16001] = { reward = { f = REP.FALLEN,          v = 160 },
                penalty = { f = REP.ORDER_OF_LIGHT, v = -128 } },
}

-- ТЗ требует явный кейс: смерть моба даёт репу «Стражам Света» и отнимает у «Падших».
-- Дефолт для мобов вне таблицы (обычные моб-кризисы ЧМ-зон):
local DEFAULT_SEEPAGE_MOB = {
    reward  = { f = REP.ORDER_OF_LIGHT, v = 16 },  -- +1 очко
    penalty = { f = REP.FALLEN,         v = -8  }, -- -0.5 очка
}

-- ---------------------------------------------------------------------
-- Хук: вызывается синхронно на world-thread после C++ OnKillCredit.
-- Важно: не делать здесь БД-запросов и yield — только чистые вычисления
-- (правило scripting docs/02 §7: Lua работает поверх кэша Player).
-- ---------------------------------------------------------------------
function on_smartai_kill_credit(player, creature)
    if not player or not creature then return end

    -- Ренегаты НЕ получают репутацию ни с одной основной фракции
    -- (зеркало инварианта ReputationMgr::ModifyReputation — двойная защита:
    -- сервер всё равно отклонит начисление, но ранний выход экономит цикл).
    if player:IsRenegate() then
        return
    end

    local entry = creature:GetEntry()
    local rule  = KILL_REP_TABLE[entry]

    -- Мобы с тегом Seepage (zone 1100/1200) без явной записи -> дефолт.
    if not rule and creature:IsSeepageMob() then
        rule = DEFAULT_SEEPAGE_MOB
    end
    if not rule then return end

    if rule.reward and rule.reward.v ~= 0 then
        dm.modify_reputation(player, rule.reward.f, rule.reward.v, "lua_kill_credit")
    end
    if rule.penalty and rule.penalty.v ~= 0 then
        dm.modify_reputation(player, rule.penalty.f, rule.penalty.v, "lua_kill_penalty")
    end
end

-- ---------------------------------------------------------------------
-- Хук перехода в ренегаты (ReputationMgr::OnBecomeRenegate).
-- Дублируем системное уведомление на языке клиента + статистику LiveOps.
-- ---------------------------------------------------------------------
function on_faction_renegade_gained(player)
    dm.send_text(player, 90110, "Вы объявлены ИЗГОЕМ. Обе фракции отвергли вас.")
    dm.analytics_event("faction.renegade", {
        guid = player:GetGUID(),
        repA = player:GetReputation(REP.ORDER_OF_LIGHT),
        repB = player:GetReputation(REP.FALLEN),
    })
end

-- Регистрация (ScriptEngine сканирует возвращаемую таблицу обработчиков).
return {
    ["smartai.on_kill_credit"]   = on_smartai_kill_credit,
    ["faction.renegade_gained"]  = on_faction_renegade_gained,
}
