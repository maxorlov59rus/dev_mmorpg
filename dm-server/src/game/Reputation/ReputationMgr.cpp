// =====================================================================
// «Чёрная Материя» — ReputationMgr.cpp
// Ядро фракционной системы + механика «Ренегат».
//
// Потоки данных (docs/02 §6):
//   Combat/Quest/Lua -> ModifyReputation -> in-memory standing (Player cache)
//     -> dirty-flag -> CharacterSaver батч в dm_characters.reputation
//     -> SMSG_REP_UPDATE (world channel, throttled 250ms)
//   Пороги рангов — из Faction.dbc; здесь только арифметика и state-machine.
//
// ВАЖНО про ренегата: флаг is_renegade — производное состояние, НЕ источник
// истины. Источник истины — standing по фракциям A/B. Поэтому RecalcRenegateFlag
// вызывается после ЛЮБОГО изменения репутации и при загрузке персона.
// =====================================================================
#include "game/Reputation/ReputationMgr.h"

#include "game/entities/Player.h"          // Player::GetSession(), m_factionRep[]
#include "game/entities/Unit.h"            // Unit::Kill pipeline
#include "game/World/World.h"              // sWorld->Rate(RATE_REPUTATION)
#include "game/Scripting/HookMgr.h"        // Lua hook dispatch (Eluna-style)
#include "game/Network/Sessions/WorldSession.h"
#include "shared/Protocol/MMOProtocol.h"   // Opcode'ы SMSG_*
#include "shared/Database/SQLClient.h"     // Prepared stmts, батч-сохранения

#include <algorithm>
#include <cmath>
#include <regex>

namespace DM
{
    namespace
    {
        // Индексы кэша standing внутри Player (параллельно таблице reputation).
        constexpr uint8_t REP_IDX_A = FACTION_ORDER_LIGHT;   // 1
        constexpr uint8_t REP_IDX_B = FACTION_FALL_DOWN;     // 2

        // Cap'ы как в TRC: нельзя уйти ниже HATED-floor и выше EXALTED.
        constexpr int32 STANDING_MIN = -60000;
        constexpr int32 STANDING_MAX =  42000;

        // Bounty за убийство ренегата: Splinters (валюта Гильдии Порога),
        // выдаётся ОБЕИМ сторонам конфликта, standing не трогает (см. .h).
        constexpr uint32 RENEGADE_KILL_BOUNTY_SPLINTERS = 25;

        [[nodiscard]] inline bool IsMainFaction(uint16_t f) noexcept
        {
            return f == FACTION_ORDER_LIGHT || f == FACTION_FALL_DOWN;
        }
    }

    ReputationMgr& ReputationMgr::Instance()
    {
        static ReputationMgr inst;
        return inst;
    }

    // ---------------------------------------------------------------------
    // ModifyReputation — единственная точка мутации standing.
    // ---------------------------------------------------------------------
    int32 ReputationMgr::ModifyReputation(Player* player, uint16_t factionId,
                                          int32 amount, const RepChangeContext& ctx)
    {
        if (!player || amount == 0 || factionId >= FACTION_COUNT_KNOWN)
            return 0;

        // Ренегат: стояние с основными фракциями ЗАМОРОЖЕНО (нельзя «отмыться»
        // гриндом — только обряд искупления / GM). Исключение: служебная
        // фракция изгоев и нейтральная Гильдия Порога — начисляем свободно.
        if (player->IsRenegate() && IsMainFaction(factionId))
        {
            if (ctx.source != RepChangeContext::Source::GMCommand)
                return 0;
        }

        int32& cur = player->MutableReputation(factionId);
        const double rate = sWorld->Rate(RATE_REPUTATION);
        const int32 applied = ApplyDelta(cur, amount, rate);
        if (applied == 0)
            return 0;

        cur += applied;
        player->SetReputationDirty(factionId);

        // Аудит (corruption.last_event — аналог): пишем только значимые события.
        if (std::abs(applied) >= 16 /* 1.0 очка */)
        {
            LOG_REP("guid={} faction={} delta={} (raw={}) src={} entry={} -> {}",
                    player->GetGUID(), factionId, applied, amount,
                    static_cast<int>(ctx.source), ctx.triggerEntry, cur);
        }

        // State-machine ренегата пересчитываем ТОЛЬКО когда задета одна из
        // двух основных фракций — это инвариант (repA<-5000 && repB<0).
        if (IsMainFaction(factionId))
            RecalcRenegateFlag(player);

        // Отправка клиенту: SMSG_FACTION_STATE дельта (throttle на сессии,
        // чтобы спам квестовых тиков не затыл world-канал — docs/03 §6.3).
        if (auto* session = player->GetSession())
            session->SendRepUpdate(factionId, cur, RankForStanding(cur));

        return applied;
    }

    int32 ReputationMgr::ApplyDelta(int32 cur, int32 delta, double rateMult) const noexcept
    {
        // Fixed-point безопасный умножитель: Q16 шаг => округление вниз по модулю
        // (как TRC для отрицательных значений сохраняет знак дельты).
        int64 scaled = std::llround(static_cast<double>(delta) * rateMult);
        if (scaled == 0)
            scaled = (delta > 0) ? 1 : -1; // гарантируем прогресс даже при x0.01

        int64 target = static_cast<int64>(cur) + scaled;
        target = std::clamp<int64>(target, STANDING_MIN, STANDING_MAX);

        const int32 clamped = static_cast<int32>(target) - cur;
        // Если clamp «съел» всю дельту — считаем изменение неприменимым.
        return clamped;
    }

    // ---------------------------------------------------------------------
    // Ренегат: проверка инварианта и переходы состояния.
    // ---------------------------------------------------------------------
    void ReputationMgr::RecalcRenegateFlag(Player* player)
    {
        if (!player)
            return;

        const int32 repA = GetReputation(player, FACTION_ORDER_LIGHT);
        const int32 repB = GetReputation(player, FACTION_FALL_DOWN);
        const bool shouldBeRenegade = CrossedRenegadeState(repA, repB);

        if (shouldBeRenegade && !player->IsRenegate())
            OnBecomeRenegate(player);
        else if (!shouldBeRenegade && player->IsRenegate())
            // Теоретически достижимо только через GM/обряд (standing заморожен),
            // но оставляем ветку для миграций и hotfix-скриптов.
            OnLeaveRenegate(player);
    }

    void ReputationMgr::OnBecomeRenegate(Player* player)
    {
        player->SetRenegate(true);
        player->SetReputationDirty_Flags(); // characters.is_renegade -> UPDATE

        // 1) Скрытый квест q_1003 «Изгой» (StarterQuests.sql): авто-выдача.
        if (auto* questMgr = player->GetQuestMgr())
            questMgr->AddQuest(player, QUEST_RENEGADE_EXILE /* id=1003, StarterQuests.sql */, /*silent*/ true);

        // 2) Визуальный каскад: Чёрная Материя «проявляется» — сервер диктует
        //    клиенту включить vein-шейдер (FactionFXManager.cpp, UE-сторона).
        if (auto* session = player->GetSession())
            session->SendFactionVisual(FACTION_RENEGADE, /*applyBlackMatterFx*/ 1);

        // 3) PvP-статус: atWarWithAll=true -> все города враждебны, все игроки
        //    видят red nameplate (обработчик в SmartAI.cpp / CanAttack).
        player->SetAtWarWithAll(true);

        // 4) Broadcast в зону + глобальный chat-канал «Изгои» (для онбординга).
        player->BroadcastText(LANG_RENEGADE_DECLARED, MSG_TYPE_NOTICE_ZONE);

        // 5) Lua-хук для контент-скриптов (контракт scripts/lua/faction/renegate.lua).
        HookMgr::Instance().CallVoid("faction.renegade_gained", player);

        LOG_REP("RENEGADE: guid={} entered exile state", player->GetGUID());
    }

    void ReputationMgr::OnLeaveRenegate(Player* player)
    {
        player->SetRenegate(false);
        player->SetAtWarWithAll(false);
        player->SetReputationDirty_Flags();

        if (auto* questMgr = player->GetQuestMgr())
            questMgr->RemoveQuest(player, QUEST_RENEGADE_EXILE);

        if (auto* session = player->GetSession())
            session->SendFactionVisual(player->GetStartFaction(), /*applyBlackMatterFx*/ 0);

        HookMgr::Instance().CallVoid("faction.renegade_revoked", player);
        LOG_REP("RENEGADE: guid={} restored to faction {}",
                player->GetGUID(), static_cast<int>(player->GetStartFaction()));
    }

    // ---------------------------------------------------------------------
    // PvP: убийство игрока. Ключевое требование ТЗ:
    //   «При убийстве игрока-ренегата обе фракции не теряют репутацию.»
    // Реализация: у victim-ренегата списание полностью пропускаем; killer
    // получает bounty без влияния на standing (нейтралитет награды).
    // ---------------------------------------------------------------------
    bool ReputationMgr::OnPlayerKilledPlayer(Player* killer, Player* victim)
    {
        if (!killer || !victim || killer == victim)
            return false;

        RepChangeContext ctx{};
        ctx.source       = RepChangeContext::Source::KillPlayer;
        ctx.triggerEntry = victim->GetGUID();

        bool any = false;

        // 1) Списания у ЖЕРТВЫ (honorable kill penalty) — только если она НЕ ренегат.
        if (!victim->IsRenegate())
        {
            const uint16_t enemyFaction =
                (victim->GetStartFaction() == FACTION_ORDER_LIGHT)
                    ? FACTION_FALL_DOWN : FACTION_ORDER_LIGHT;

            const int32 loss = ModifyReputation(victim, enemyFaction,
                                                -HONORABLE_KILL_REP_LOSS, ctx);
            any |= (loss != 0);
        }
        // victim->IsRenegate(): обе фракции молча сохраняют standing. Инвариант
        // ренегата уже замораживает начисления в ModifyReputation, но явный skip
        // здесь нужен ради аудита: логируем «no-loss on renegade kill».
        else
        {
            LOG_REP("PvP: victim {} is renegade -> no reputation loss for both factions",
                    victim->GetGUID());
        }

        // 2) Награда убийце: standing НЕ двигается (чтобы PvP-гринд не ломал
        //    порог ренегата), начисляем только Splinters (через Currency system).
        killer->AddCurrency(CURRENCY_SPLINTERS, RENEGADE_KILL_BOUNTY_SPLINTERS,
                            /*reason*/ "pvp_kill");

        return any;
    }

    // ---------------------------------------------------------------------
    // DB layer. Standing хранится в dm_characters.reputation; денормализация
    // в characters.* нужна чтобы WorldSession не делал JOIN при каждом входе.
    // ---------------------------------------------------------------------
    void ReputationMgr::LoadFromDB(Player* player)
    {
        if (!player)
            return;

        auto result = CharacterDatabase().Query(
            "SELECT faction_id, standing FROM reputation WHERE guid = ?",
            player->GetGUID());

        while (result->NextRow())
        {
            const uint16_t fid = result->GetUInt16(0);
            if (fid < FACTION_COUNT_KNOWN)
                player->SetReputationRaw(fid, result->GetInt32(1));
        }

        // Денормализованные колонки (гарантия консистентности после load):
        RecalcRenegateFlag(player);
    }

    void ReputationMgr::SaveToDB(Player* player) const
    {
        if (!player || !player->HasReputationDirty())
            return;

        // UPSERT по всем грязным фракциям + one-shot обновление флагов.
        for (uint16_t f = 0; f < FACTION_COUNT_KNOWN; ++f)
        {
            if (!player->IsReputationDirty(f))
                continue;

            CharacterDatabase().Execute(
                "INSERT INTO reputation (guid, faction_id, standing) VALUES (?, ?, ?) "
                "ON DUPLICATE KEY UPDATE standing = VALUES(standing)",
                player->GetGUID(), f, GetReputation(player, f));
        }

        CharacterDatabase().Execute(
            "UPDATE characters SET "
            "  reputation_faction_1 = ?, reputation_faction_2 = ?, is_renegade = ? "
            "WHERE guid = ?",
            GetReputation(player, FACTION_ORDER_LIGHT),
            GetReputation(player, FACTION_FALL_DOWN),
            player->IsRenegate() ? 1 : 0,
            player->GetGUID());

        player->ClearReputationDirty();
    }

    void ReputationMgr::UpdateSaveTimers()
    {
        // Как TRC CharacterSaver: раз в интервал собираем всех dirty-игроков
        // одного mapserver'а и делаем один батч транзакцией.
        static uint32 lastTick = 0;
        const uint32 now = WorldTimer::getMSTime();
        if (WorldTimer::diffTime(lastTick, now) < REP_SAVE_INTERVAL_MS)
            return;
        lastTick = now;

        World().ForEachOnlinePlayer([](Player* p)
        {
            if (p && p->HasReputationDirty())
                Instance().SaveToDB(p);
        });
    }

    // ---------------------------------------------------------------------
    // Геттеры (read-only view поверх кэша Player).
    // ---------------------------------------------------------------------
    int32 ReputationMgr::GetReputation(const Player* player, uint16_t factionId) const
    {
        if (!player || factionId >= FACTION_COUNT_KNOWN)
            return 0;
        return player->GetReputationCached(factionId);
    }

    RepRank ReputationMgr::GetRank(const Player* player, uint16_t factionId) const
    {
        return RankForStanding(GetReputation(player, factionId));
    }

    bool ReputationMgr::IsRenegate(const Player* player) const
    {
        return player && player->IsRenegate();
    }

} // namespace DM
