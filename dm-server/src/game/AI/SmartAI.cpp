// =====================================================================
// «Чёрная Материя» — SmartAI.cpp (фрагмент: реакция NPC на фракции/ренегатов)
// Аналог TrinityCore src/server/scripts/SmartAI + кастомные conditions.
//
// Интеграция: этот TU компилируется в target dm-scripts-core; события
// CreatureEvents::KILL_CREDIT / PLAYER_ENTERING_RANGE / GOSSIP_SELECT
// маршрутизируются через SmartScript, а быстрые проверки (агрессия к
// ренегатам) вынесены в Unit::CanAttack hot path — см. docs/02 §5.
// =====================================================================
#include "game/AI/SmartAI.h"

#include "game/AI/AggressorAI.h" // базовая машина агрессии как TRC
#include "game/Maps/Map.h"
#include "game/Reputation/ReputationMgr.h"
#include "game/entities/Creature.h"
#include "game/entities/Player.h"
#include "game/entities/Unit.h"
#include "shared/Text/ChatTextStore.h" // Lang/*.sql -> TextId lookup

namespace DM {
namespace {
// ---------- Конфигурация aggressiveness к ренегатам ----------
// unit_flags2 | UNIT_FLAGS2_ATTACK_RENEGADE_ON_SIGHT (бит 21):
// ставится в creature_template для стражей городов (NPC_Config.sql).
constexpr uint32 UNIT_FLAG2_ATTACK_RENEGADE = 1u << 21;

// Радиус «входа в город» для автоматической агрессии стража.
// Держим в fixed-point Q16 (docs/02 §6): 25 м = 25*65536.
constexpr int32 GUARD_AGGRO_RADIUS_CM_FP = 25 * 65536;

[[nodiscard]] inline bool IsCityGuard(const Creature *c) noexcept {
    return c && (c->GetTemplate()->UnitFlags2 & UNIT_FLAG2_ATTACK_RENEGADE) != 0;
}
} // namespace

// -----------------------------------------------------------------
// Hot path: переопределение правила CanAttack для ренегатов.
// Вызывается из ThreatManager при каждом candidate-scan => должен быть
// безаллокационным и не трогать БД (standing уже в кэше Player).
// -----------------------------------------------------------------
bool SmartAI::CanAttackRenegade(const Creature *creature, const Player *player) {
    if (!creature || !player)
        return false;

    // Ренегат объявляет войну ВСЕМ фракционным NPC автоматически.
    if (!player->IsRenegate())
        return false;

    // Страж города (city guard) атакует ренегата НА ВХОДЕ в городскую зону
    // (см. требование ТЗ) — реализуется через флаг шаблона + радиус ниже.
    if (IsCityGuard(creature))
        return true;

    // Обычные фракционные моб: агрессия только если standing <= HATED
    // или NPC настроен на faction-war (CreatureTemplate::FactionGroup).
    const uint16_t npcFaction = creature->GetFactionId();
    if (npcFaction == FACTION_ORDER_LIGHT || npcFaction == FACTION_FALL_DOWN) {
        const RepRank rank = GetReputationMgr().GetRank(player, npcFaction);
        return rank <= RANK_HOSTILE; // hostile/hated -> attack
    }

    // Нейтральные (Гильдия Порога, служебная фракция изгоев) — не трогают.
    return false;
}

// -----------------------------------------------------------------
// PLAYER_ENTERING_RANGE event hook: триггер агрессии стража именно
// «на входе», а не с момента спавна (иначе стражи аггрили бы через
// стены — классический баг TRC-подобных серверов).
// -----------------------------------------------------------------
void SmartAI::OnPlayerEnteringRange(Creature *me, Player *player, const WorldLocation & /*loc*/) {
    if (!me || !player)
        return;

    if (!IsCityGuard(me))
        return;

    if (!player->IsRenegate())
        return;

    // Проверка дистанции в fixed-point (избегаем sqrt/float): сравним
    // квадраты расстояний в cm^2 (Q32 via int64).
    const int32 dx = player->GetPos().x - me->GetPos().x;
    const int32 dy = player->GetPos().y - me->GetPos().y;
    const int64 dist2 = (static_cast<int64>(dx) * dx + static_cast<int64>(dy) * dy) >> 16;
    const int64 r = GUARD_AGGRO_RADIUS_CM_FP;
    if (dist2 > (r * r >> 16))
        return;

    // Агрессивная реплика перед атакой (TextID из Lang, см. NPC_Config.sql).
    me->MonsterTextEmote(TEXT_GUARD_RENEGADE_CHALLENGE, player,
                         /*isLanguage*/ true);

    // Назначаем целью и включаем преследование в рамках городской зоны.
    me->AI()->AttackStart(player);
    me->SetThreatState(THREAT_STATE_AGGRO_FULL);

    // Broadcast SMSG_CREATURE_AGGRO (AOI-сегмент, чтобы соседние игроки
    // видели анимацию «страж заметил изгоя» — важно для mobile tiering).
    me->GetMap()->SendToArea(me, SMSG_CREATURE_AGGRO, me->GetGUID(), player->GetGUID());
}

// -----------------------------------------------------------------
// GOSSIP_SELECT: ветвление диалога по рангу репутации.
// condition_id в gossip_menu_option (DB_Schema.sql) фильтрует опции
// ДО отправки клиенту; здесь — runtime-guard против packet-replay
// (клиент может прислать id опции, которую ему не показывали).
// -----------------------------------------------------------------
bool SmartAI::GossipOptionSatisfiesCondition(Player *player, uint32 conditionId) {
    switch (conditionId) {
    case COND_REP_FRIENDLY_ANY:
        return GetReputationMgr().GetRank(player, player->GetStartFaction()) >= RANK_FRIENDLY;
    case COND_REP_HONORED_OR_ORDER:
        return GetReputationMgr().GetRank(player, FACTION_ORDER_LIGHT) >= RANK_HONORED;
    case COND_IS_RENEGADE:
        return player->IsRenegate();
    case COND_NOT_RENEGADE:
        return !player->IsRenegate();
    default:
        return true; // COND_NONE
    }
}

void SmartAI::OnGossipSelect(Creature *me, Player *player, uint32 menuId, uint32 optionId) {
    if (!me || !player)
        return;

    const GossipOption *opt = sGossipStore->GetOption(menuId, optionId);
    if (!opt)
        return;

    if (!GossipOptionSatisfiesCondition(player, opt->ConditionId)) {
        // Replay/подмена — мягко закрываем окно + audit-лог (античит docs/03 §8).
        player->GetSession()->SendGossipClose(menuId);
        LOG_WARN("Gossip condition bypass attempt: guid={} menu={} opt={}", player->GetGUID(),
                 menuId, optionId);
        return;
    }

    // Маршрутизация action'ов (Trade / Quests / Reputation — см. ТЗ артефакт 4).
    switch (opt->ActionType) {
    case GOSSIP_ACTION_TRADE:
        player->GetSession()->SendListInventory(me->GetGUID());
        break;
    case GOSSIP_ACTION_QUEST_MENU:
        player->GetSession()->SendQuestMenu(me->GetGUID());
        break;
    case GOSSIP_ACTION_REP_INFO:
        SendReputationSummary(me, player);
        break;
    case GOSSIP_ACTION_RENEGADE_BLACKMARKET:
        // Доступ к «Черному рынку» (награда q_1003) — торговый лот
        // с нейтральными товарами, доступен ТОЛЬКО ренегатам.
        if (player->IsRenegate())
            player->GetSession()->SendListInventory(me->GetGUID(),
                                                    /*vendorGroup*/ VENDOR_BLACK_MARKET);
        break;
    default:
        break;
    }
}

// -----------------------------------------------------------------
// KILL_CREDIT event: начисление репутации за убийство моба + прогресс квестов.
// Числа (amount/rank) приходят из SmartAIMain таблицы (DBC-like config),
// аналог TRC smart_scripts params[1..2]; дробные значения умножены на 16.
// -----------------------------------------------------------------
void SmartAI::OnKillCredit(Creature *killed, Player *killer, const SmartEventParams &params) {
    if (!killed || !killer)
        return;

    RepChangeContext ctx{};
    ctx.source = RepChangeContext::Source::KillCreature;
    ctx.triggerEntry = killed->GetEntry();

    // params.repFactionA / repAmountA — положительная награда «своей» фракции;
    // params.repFactionB / repAmountB — штраф враждебной (обычно отрицательный).
    if (params.repFactionA != 0 && params.repAmountA != 0)
        GetReputationMgr().ModifyReputation(killer, params.repFactionA, params.repAmountA, ctx);
    if (params.repFactionB != 0 && params.repAmountB != 0)
        GetReputationMgr().ModifyReputation(killer, params.repFactionB, params.repAmountB, ctx);

    // Квесты lvl 1-5: kill-goal прогресс (q_1002 x5 Слизень, q_1004 x6 Жук,
    // q_1006 Полый, q_1007 Тень). QuestMgr сверяет RequiredCreatureKill1 у
    // активных квестов и шлёт SMSG_QUEST_PROGRESS / SMSG_QUEST_TURN_IN_HINT.
    killer->GetQuestMgr()->KilledCreature(killed->GetEntry(), /*count*/ 1);

    // Lua-overridable хук (scripts/lua/smartai/*): даёт контент-команде
    // менять логику без рекомпиляции ядра — прямой аналог Eluna Scripts.
    HookMgr::Instance().CallVoid("smartai.on_kill_credit", killer, killed);
}

// -----------------------------------------------------------------
// ДИАЛОГИ МОБОВ LVL 1-5 (StarterContent_L1_5.sql §1): реплики аггро/ранения/
// смерти. TextID'ы лежат в creature_template.{Aggro,Wound,Death}TextID
// (broadcast_text 910xx), доставка — SMSG_CREATURE_TEXT (defs 0x2014) через
// MonsterTextEmote -> Map::SendToArea (AOI). Кулдауны держим в AiTimer, чтобы
// не спамить трафик на массовых киллах фарм-зон.
// -----------------------------------------------------------------
void SmartAI::OnEnterCombat(Creature *me, Unit *victim) {
    if (!me || !victim)
        return;
    const uint32 textId = me->GetTemplate()->AggroTextID;
    if (textId == 0)
        return;
    // однократно за боевой цикл (сбрасывается в OnCombatStop)
    if (me->GetAiTimer(AI_TIMER_AGGRO_TEXT) != 0)
        return;
    me->SetAiTimer(AI_TIMER_AGGRO_TEXT, 1); // mark: shown this combat
    me->MonsterTextEmote(textId, victim->GetPlayer(), /*isLanguage*/ true);
}

void SmartAI::OnCombatStop(Creature *me) {
    if (me)
        me->SetAiTimer(AI_TIMER_AGGRO_TEXT, 0); // разрешить реплику при следующем аггро
}

void SmartAI::OnHealthPercent(Creature *me, uint8 pctHp) {
    if (!me || pctHp > 30)
        return; // «ранен» — ниже 30%
    const uint32 textId = me->GetTemplate()->WoundTextID;
    if (textId == 0 || me->GetAiTimer(AI_TIMER_WOUND_TEXT) != 0)
        return;
    me->SetAiTimer(AI_TIMER_WOUND_TEXT, 1);
    me->MonsterTextEmote(textId, me->GetVictim() ? me->GetVictim()->GetPlayer() : nullptr, true);
}

void SmartAI::OnJustDied(Creature *me) {
    if (!me)
        return;
    const uint32 textId = me->GetTemplate()->DeathTextID;
    if (textId != 0)
        me->MonsterTextEmote(textId, me->GetVictim() ? me->GetVictim()->GetPlayer() : nullptr,
                             true);
}

// -----------------------------------------------------------------
// Агрессивные фразы по порогу ненависти (ТЗ: "< Hated -> TextID").
// Реализация: periodical emote с кулдауном, чтобы не спамить AOI-трафик.
// -----------------------------------------------------------------
void SmartAI::UpdateHostileTaunts(Creature *me, uint32 diffMs) {
    static const std::unordered_map<ObjectGuid, uint32> lastTaunt; // упрощённо
    if (!me || !me->GetVictim() || !me->GetVictim()->GetPlayer())
        return;

    Player *victim = me->GetVictim()->GetPlayer();
    const uint16_t npcFaction = me->GetFactionId();
    if (npcFaction != FACTION_ORDER_LIGHT && npcFaction != FACTION_FALL_DOWN)
        return;

    const RepRank rank = GetReputationMgr().GetRank(victim, npcFaction);
    if (rank > RANK_HATED)
        return;

    // Кулдаун 8 секунд на реплику (проверка в me->GetAiTimer()).
    if (me->GetAiTimer(AI_TIMER_TAUNT) < diffMs) {
        me->SetAiTimer(AI_TIMER_TAUNT, 8000);
        me->MonsterTextEmote(TEXT_HOSTILE_TAUNT[npcFaction], victim, true);
    }
}

} // namespace DM
