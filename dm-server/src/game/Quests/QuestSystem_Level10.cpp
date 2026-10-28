// =============================================================================
// «Чёрная Материя» — dm-server / game/Quests/QuestSystem_Level10.cpp
// Квесты 6–10: автовыдача квеста специализации на 10 уровне, выбор спеки,
// выдача способностей (GrantSpecAbilities), LFG-гейт и «Рупор призыва».
//
// Паттерны заимствованы из QuestGossipHandlers.cpp (регистрация обработчиков)
// и PlayerClassMgr.cpp (кэш из БД + TODO(KERNEL)-заглушки до линковки с
// game/Entities/Player.h — тот же контракт заглушек, см. docs/02 §Структура).
//
// Wire-опкоды (proto/opcodes.defs v1.5):
//   SMSG_SPEC_SELECTION_OPEN   0x2023 s2c { u8 class_id arr<u32>{4} spec_ids }
//   CMSG_CHOOSE_SPEC           0x2024 c2s { u8 spec_id }
//   SMSG_SPEC_CHOSEN           0x2025 s2c { u8 spec_id u8 lfg_role_mask }
//   SMSG_LEARNED_SPELLS_BATCH  0x2026 s2c { arr<u32>{16} spell_ids u8 source }
//   CMSG_JOIN_LFG              0x2020 c2s { u16 dungeon_id u8 role_flags }
//   SMSG_LFG_QUEUE_STATE       0x2021 s2c { u8 state u16 dungeon_id u32 est_wait_ms }
//   SMSG_LFG_ROLE_CHECK        0x2022 s2c { u16 dungeon_id u8 required_roles u8 ok }
// =============================================================================
#include "QuestSystem_Level10.h"

#include "Database/CharacterDatabase.h"
#include "Game/AI/SmartAI.h"
#include "Game/Classes/PlayerClassMgr.h"
#include "Network/OpcodeDispatcher.h"
#include "Shared/Protocol/Opcodes.h"

#include <cstdio>

namespace DM {
namespace Game {

// -----------------------------------------------------------------------------
// Синглтон
// -----------------------------------------------------------------------------
SpecQuestMgr &SpecQuestMgr::instance() {
    static SpecQuestMgr s;
    return s;
}

bool SpecQuestMgr::loadFromDb() {
    _specSpells.clear();

    // playercreateinfo_specs: ClassID, SpecID, SpellID_1..SpellID_4
    auto res = CharDB().Query(
        "SELECT class_id, spec_id, spell_id_1, spell_id_2, spell_id_3, spell_id_4 "
        "FROM playercreateinfo_specs ORDER BY class_id, spec_id");
    while (auto row = res.NextRow()) {
        SpecSpellsRow r;
        r.classId = static_cast<uint8_t>(row->GetUInt32(0));
        r.specId = static_cast<uint8_t>(row->GetUInt32(1));
        for (int i = 2; i <= 5; ++i) {
            uint32_t sp = row->GetUInt32(i);
            if (sp != 0)
                r.spellIds.push_back(sp);
        }
        _specSpells.push_back(std::move(r));
    }

    std::printf("[SpecQuestMgr] загружено %zu наборов способностей специализаций\n",
                _specSpells.size());
    return true;
}

const SpecQuestMgr::SpecSpellsRow *SpecQuestMgr::FindSpecSpells(uint8_t classId,
                                                                uint8_t specId) const {
    for (const auto &row : _specSpells)
        if (row.classId == classId && row.specId == specId)
            return &row;
    return nullptr;
}

// -----------------------------------------------------------------------------
// OnLevelChanged — ядро автовыдачи квеста «Путь избранного» (q 1104)
// -----------------------------------------------------------------------------
void SpecQuestMgr::OnLevelChanged(Player *player, uint8 oldLevel, uint8 newLevel) {
    if (!player || newLevel < LEVEL_SPEC_UNLOCK || oldLevel >= LEVEL_SPEC_UNLOCK)
        return; // пересекли ровно один раз

    // TODO(KERNEL): Player API (см. PlayerClassMgr.cpp, те же заглушки):
    //   uint8 cls = player->GetClass();
    //   bool hasSpec = player->GetActiveSpec() != SPEC_NONE;
    //   bool qDone = player->GetQuestStatus(QUEST_PATH_CHOSEN) == QUEST_STATUS_REWARDED;
    //   bool qActive = player->GetQuestStatus(QUEST_PATH_CHOSEN) == QUEST_STATUS_INCOMPLETE;
    const uint8_t cls = 1;      // TODO(KERNEL): player->GetClass()
    const bool hasSpec = false; // TODO(KERNEL): player->GetActiveSpec() != SPEC_NONE
    const bool qDone = false;   // TODO(KERNEL): quest status lookup
    const bool qActive = false; // TODO(KERNEL): quest status lookup

    if (hasSpec || qDone || qActive)
        return; // уже выбрано / уже пройден — не дублируем

    // Автовыдача стартового квеста выбора спеки (аналог TRC QuestMgr::AddQuest
    // без NPC-привязки; SpecialFlags |= HIDDEN_TRACKER показан в SQL-контенте).
    // TODO(KERNEL): player->GetQuestMgr()->AddQuest(player, QUEST_PATH_CHOSEN, /*auto=*/true);
    std::printf("[SpecQuestMgr] lvl%u: автовыдан квест %u (класс %u)\n", newLevel,
                QUEST_PATH_CHOSEN, cls);

    // Клиент должен открыть модалку UI_SpecSelection сразу после тоста уровня.
    SendSpecSelectionOpen(player);
}

void SpecQuestMgr::SendSpecSelectionOpen(Player *player) {
    // TODO(KERNEL): заменить на реальный Player API.
    const uint8_t cls = 1; // player->GetClass()

    PacketBuilder p(SMSG_SPEC_SELECTION_OPEN);
    p.WriteU8(cls);
    // Список доступных спек для класса (клиент строит панели по DataTable).
    const auto specs = PlayerClassMgr::instance().GetSpecsForClass(cls);
    p.WriteU8(static_cast<uint8_t>(specs.size()));
    for (const auto &s : specs)
        p.WriteU32(s.specId);
    // TODO(KERNEL): player->GetSession()->SendPacket(p.Build());
    (void)player;
}

// -----------------------------------------------------------------------------
// ChooseSpec — серверный guard (всегда проверяем, клиенту не доверяем)
// -----------------------------------------------------------------------------
bool SpecQuestMgr::ChooseSpec(Player *player, uint8 specId, const char *&outReason) {
    outReason = nullptr;
    if (!player) {
        outReason = "no_player";
        return false;
    }

    // TODO(KERNEL): const uint8 cls = player->GetClass();
    //               const uint8 lvl = player->GetLevel();
    const uint8_t cls = 1;     // placeholder до Entities/Player.h
    const uint8_t lvl = LEVEL_SPEC_UNLOCK;

    if (lvl < LEVEL_SPEC_UNLOCK) {
        outReason = "level_too_low";
        return false;
    }
    if (!PlayerClassMgr::instance().SpecBelongsToClass(specId, cls)) {
        outReason = "spec_not_for_class"; // packet-spoof защита
        return false;
    }
    // TODO(KERNEL): if (player->GetActiveSpec() != SPEC_NONE) { outReason = "spec_locked"; return false; }

    // 1) Пишем активную спеку (char_specs.spec_active) — переживает релогаг.
    CharDB().Execute("UPDATE characters SET spec_active = ? WHERE guid = ?", specId,
                     /*guid*/ 0 /* TODO(KERNEL): player->GetGUID() */);

    // 2) Выдаём способности спеки (батч SMSG_LEARNED_SPELLS_BATCH внутри).
    std::vector<uint32_t> learned = GrantSpecAbilities(player, specId);
    if (learned.empty()) {
        outReason = "no_spec_spells"; // данные БД неполные — не блокируем выбор,
                                      // но сигнализируем в лог контента
        std::printf("[SpecQuestMgr] WARNING: пусто playercreateinfo_specs class=%u spec=%u\n", cls,
                    specId);
    }

    // 3) Ачивка «Первая специализация» -> гейтер LFG (lfg_dungeons + HasAchieved).
    // TODO(KERNEL): player->EarnAchievement(ACHIEVEMENT_FIRST_SPEC);
    (void)ACHIEVEMENT_FIRST_SPEC;

    // 4) SMSG_SPEC_CHOSEN: роль для LFG + спека (клиент: превью экипировки
    //    щит+меч / двуручник, подсветка action bar золотом 10 сек).
    PacketBuilder p(SMSG_SPEC_CHOSEN);
    p.WriteU8(specId);
    p.WriteU8(GetLfgRoleForPlayer(player));
    // TODO(KERNEL): player->GetSession()->SendPacket(p.Build());

    // 5) Цепочка: после сдачи q_1104 выдаём q_1105 «Проверка боем» (обучающий
    //    инстанс через LFG) — делает Lua OnComplete (quest_1004_spec_choice.lua).
    return true;
}

// -----------------------------------------------------------------------------
// GrantSpecAbilities — ядро выдачи спеллов (Lua-мост Player:GrantSpecAbilities)
// -----------------------------------------------------------------------------
std::vector<uint32_t> SpecQuestMgr::GrantSpecAbilities(Player *player, uint8 specId) {
    std::vector<uint32_t> learned;
    if (!player)
        return learned;

    // TODO(KERNEL): const uint8 cls = player->GetClass();
    const uint8_t cls = 1;

    const SpecSpellsRow *row = FindSpecSpells(cls, specId);
    if (!row)
        return learned;

    // Смена спеки (M2, тренер): сначала снимаем ауры/спеллы предыдущей спеки.
    // TODO(KERNEL):
    //   for (uint32 oldSpell : GetSpecSpellsOfPrevious(player))
    //       player->RemoveSpell(oldSpell);            // DELETE FROM char_spells
    //   player->RemoveAurasBySrc(SPELL_SRC_SPEC);     // чистим char_auras

    for (uint32_t spellId : row->spellIds) {
        // TODO(KERNEL): if (player->HasSpell(spellId)) continue;
        //              player->AddSpell(spellId, SPELL_SRC_SPEC); // INSERT char_spells
        learned.push_back(spellId);
    }

    if (!learned.empty())
        SendLearnedSpellsBatch(player, learned);
    return learned;
}

void SpecQuestMgr::SendLearnedSpellsBatch(Player *player, const std::vector<uint32_t> &spellIds) {
    PacketBuilder p(SMSG_LEARNED_SPELLS_BATCH);
    const uint8_t count = static_cast<uint8_t>(std::min<size_t>(spellIds.size(), 16));
    p.WriteU8(count);
    for (uint8_t i = 0; i < count; ++i)
        p.WriteU32(spellIds[i]);
    p.WriteU8(/*source=*/3); // 3 = spec (зеркало EMMOSpellSource клиента)
    // TODO(KERNEL): player->GetSession()->SendPacket(p.Build());
    (void)player;
}

// -----------------------------------------------------------------------------
// LFG-роль и гейт очереди
// -----------------------------------------------------------------------------
uint8_t SpecQuestMgr::GetLfgRoleForPlayer(const Player *player) const {
    // Роль определяется первичным статом спеки (class_specs.primary_stat):
    //   STA(3) -> танк; класс Жрец -> хил; иначе DPS.
    // TODO(KERNEL): const SpecId spec = player->GetActiveSpec(); const uint8 cls = player->GetClass();
    const uint8_t cls = 1; // placeholder
    const ClassSpecInfo *info = nullptr;
    // TODO(KERNEL): info = PlayerClassMgr::instance().FindSpec(player->GetActiveSpec());
    (void)cls;

    if (!info)
        return LFG_ROLE_DPS; // без спеки в очередь не пускаем (CanJoinLfg), сюда не доходим
    if (info->primaryStat == 3)
        return LFG_ROLE_TANK;
    if (info->classId == CLASS_PRIEST)
        return LFG_ROLE_HEALER;
    return LFG_ROLE_DPS;
}

bool SpecQuestMgr::CanJoinLfg(const Player *player, uint16_t dungeonId,
                              const char *&outReason) const {
    outReason = nullptr;
    if (!player) {
        outReason = "no_player";
        return false;
    }

    // Конфиг данжа: lfg_dungeons (ID, MinLevel, MaxLevel, MapID) — см. SQL артефакт 2.
    struct DungeonCfg {
        uint16_t id;
        uint8_t minLevel;
        uint8_t maxLevel;
        uint32_t mapId;
    };
    static const DungeonCfg kDungeons[] = {
        {1, 10, 15, 901}, // Пещеры Испытаний (обучающий, q_1105)
    };
    const DungeonCfg *cfg = nullptr;
    for (const auto &d : kDungeons)
        if (d.id == dungeonId)
            cfg = &d;
    if (!cfg) {
        outReason = "unknown_dungeon";
        return false;
    }

    // TODO(KERNEL): const uint8 lvl = player->GetLevel();
    //               if (!player->HasAchieved(ACHIEVEMENT_FIRST_SPEC)) ...
    //               if (player->GetActiveSpec() == SPEC_NONE) ...
    const uint8_t lvl = LEVEL_SPEC_UNLOCK; // placeholder
    if (lvl < cfg->minLevel || lvl > cfg->maxLevel) {
        outReason = "level_out_of_range";
        return false;
    }
    // Гейтер промпта: Level >= 10 И HasAchieved(ACHIEVEMENT_FIRST_SPEC).
    // TODO(KERNEL): if (!player->HasAchieved(ACHIEVEMENT_FIRST_SPEC)) { outReason = "spec_not_chosen"; return false; }

    return true;
}

// -----------------------------------------------------------------------------
// Сетевые обработчики (регистрация на boot worldserver)
// -----------------------------------------------------------------------------
void RegisterSpecQuestHandlers(Network::OpcodeDispatcher &disp, SpecQuestMgr &mgr) {
    // CMSG_CHOOSE_SPEC { u8 spec_id } — только после серверного квеста 1104.
    disp.Register(CMSG_CHOOSE_SPEC, [&mgr](PlayerSession &ps, const BinaryPacketReader &r) {
        uint8 specId = 0;
        if (!r.ReadU8(specId))
            return; // битый пакет — игнор (античит-паттерн gossip-блока)

        const char *reason = nullptr;
        if (!mgr.ChooseSpec(ps.GetPlayer(), specId, reason)) {
            std::printf("[SpecQuest] CMSG_CHOOSE_SPEC rejected: %s (spec=%u guid=%llu)\n",
                        reason ? reason : "?", specId, static_cast<unsigned long long>(ps.GetGuid()));
            // TODO(KERNEL): SMSG_SPEC_ERROR (M1 — лог; M2 — опкод 0x2027)
            return;
        }
        // Ответ уже отправлен внутри ChooseSpec (SMSG_SPEC_CHOSEN + batch).
    });

    // CMSG_JOIN_LFG { u16 dungeon_id u8 role_flags }
    disp.Register(CMSG_JOIN_LFG, [&mgr](PlayerSession &ps, const BinaryPacketReader &r) {
        uint16 dungeonId = 0;
        uint8 roleFlags = 0;
        if (!r.ReadU16(dungeonId) || !r.ReadU8(roleFlags))
            return;

        const char *reason = nullptr;
        PacketBuilder p(SMSG_LFG_QUEUE_STATE);
        if (!mgr.CanJoinLfg(ps.GetPlayer(), dungeonId, reason)) {
            p.WriteU8(/*state=*/0 /*rejected*/);
            p.WriteU16(dungeonId);
            p.WriteU32(0);
            // TODO(KERNEL): ps.SendPacket(p.Build());
            std::printf("[LFG] join rejected guid=%llu dungeon=%u reason=%s\n",
                        static_cast<unsigned long long>(ps.GetGuid()), dungeonId,
                        reason ? reason : "?");
            return;
        }

        // Проверка роли перед матчем — SMSG_LFG_ROLE_CHECK (клиент LFGQueueWidget
        // показывает Танк/Хил/ДД и запрещает вход без нужной роли).
        const uint8 myRole = mgr.GetLfgRoleForPlayer(ps.GetPlayer());
        PacketBuilder rc(SMSG_LFG_ROLE_CHECK);
        rc.WriteU16(dungeonId);
        rc.WriteU8(LFG_ROLE_TANK | LFG_ROLE_HEALER | LFG_ROLE_DPS); // требуемые (PvE: любая)
        rc.WriteU8((myRole & (LFG_ROLE_TANK | LFG_ROLE_HEALER | LFG_ROLE_DPS)) ? 1 : 0);
        // TODO(KERNEL): ps.SendPacket(rc.Build());

        // state=1 queued; est_wait — из метрик QueueMgr (M1: константа).
        p.WriteU8(/*state=*/1);
        p.WriteU16(dungeonId);
        p.WriteU32(45000);
        // TODO(KERNEL): ps.SendPacket(p.Build());
        // TODO(M2): QueueMgr::Enqueue(guid, dungeonId, roleMask) + матчмейкинг-цикл.
    });
}

} // namespace Game

// -----------------------------------------------------------------------------
// Хук ScriptMgr: проброс level-up в SpecQuestMgr (регистрируется в HookMgr
// рядом с ClassHooks::OnLevelChanged — порядок важен: сначала class_spells,
// затем спека-квест).
// -----------------------------------------------------------------------------
void RegisterSpecLevelHook() {
    // TODO(KERNEL): HookMgr::Instance().RegisterListener("player.on_level_changed", [](Player* p, uint8 o, uint8 n){
    //     Game::SpecQuestMgr::instance().OnLevelChanged(p, o, n);
    // });
}

} // namespace DM
