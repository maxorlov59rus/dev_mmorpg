// =============================================================================
// «Чёрная Материя» — dm-server / game/Quests/QuestSystem_Level10.h
// Квестовая система 6–10: выбор специализации на 10 уровне + LFG-разблокировка
// (промпт «Квестовая система 6–10: Инстансы и Специализации», артефакт 1).
//
// Ответственности:
//  1) OnLevelChanged: при достижении LevelSpecUnlock (10) автоматически выдаём
//     стартовый квест выбора спеки (QUEST_SPEC_SELECTION_* по классу) и шлём
//     клиенту SMSG_SPEC_SELECTION_OPEN (wire 0x2023) — модалка UI_SpecSelection.
//  2) CMSG_CHOOSE_SPEC (0x2024): серверная валидация SpecID×ClassID
//     (PlayerClassMgr::SpecBelongsToClass), запись char_specs, выдача способностей
//     специализации (playercreateinfo_specs) батчем SMSG_LEARNED_SPELLS_BATCH,
//     ачивка ACHIEVEMENT_FIRST_SPEC -> разблокировка LFG-очереди.
//  3) GrantSpecAbilities(): ядро выдачи/замены спеллов спеки (Lua-мост:
//     Player:GrantSpecAbilities(SpecID) в quest_1004_spec_choice.lua).
//  4) Выдача предмета «Рупор призыва» (LFG Token, entry 5120) при сдаче q_1005.
//
// Wire-контракт: proto/opcodes.defs v1.5 (блок 0x2020..0x2027).
// ID-контент совпадает с sql/upgrades/Insert_Quests_6_to_10.sql.
// =============================================================================
#pragma once

#include <cstdint>
#include <vector>

namespace DM {

class Player;

namespace Game {

// -----------------------------------------------------------------------------
// Канонические ID квестов 6–10 (sql/upgrades/Insert_Quests_6_to_10.sql).
// Внимание: 1001..1007 заняты стартовой линией L1–5 (StarterContent_L1_5.sql);
// цепочка 6–10 использует диапазон 1100+ (см. docs/09 §Нумерация).
// -----------------------------------------------------------------------------
enum StarterQuestId6to10 : uint32_t {
    QUEST_FIRST_TRIALS = 1101,   // 6 ур.: «Первые испытания» (kill 5 слизней)
    QUEST_DELIVERY_REPORT = 1102,// 7 ур.: «Доставка донесения» (travel-goal)
    QUEST_FRONT_SUPPLIES = 1103, // 8 ур.: «Снабжение фронта» (gather 10 ресурсов)
    QUEST_PATH_CHOSEN = 1104,    // 10 ур.: «Путь избранного» (выбор специализации)
    QUEST_TRIAL_BY_FIRE = 1105,  // 10 ур.: «Проверка боем» (инстанс через LFG)
};

// Стартовые квесты выбора спеки по классу (auto-grant на 10 уровне).
enum SpecSelectionQuestId : uint32_t {
    QUEST_SPEC_SELECTION_WARRIOR = 1104, // единый квест, ветка gossip по ClassID
    QUEST_SPEC_SELECTION_MAGE = 1104,
    QUEST_SPEC_SELECTION_ROGUE = 1104,
};

// Предмет «Рупор призыва» — LFG Token (item_template.entry 5120, BoP).
constexpr uint32_t ITEM_LFG_HORN = 5120;

// Ачивка «Первая специализация» — гейтер LFG (lfg_dungeons.MinLevel + эта ачивка).
constexpr uint32_t ACHIEVEMENT_FIRST_SPEC = 90010;

// Уровень разблокировки специализации/LFG (класс-агностичный, конфиг).
constexpr uint8_t LEVEL_SPEC_UNLOCK = 10;

// Роли для LFG (битовая маска; определяется активной специализацией).
enum LfgRoleFlags : uint8_t {
    LFG_ROLE_NONE = 0,
    LFG_ROLE_TANK = 1 << 0,
    LFG_ROLE_HEALER = 1 << 1,
    LFG_ROLE_DPS = 1 << 2,
};

class SpecQuestMgr {
public:
    static SpecQuestMgr &instance();

    // Инициализация: загрузка playercreateinfo_specs (ClassID->SpecID->SpellIDs)
    // и маппинга SpecID->роли из БД. Вызывается на boot worldserver.
    bool loadFromDb();

    // === Хук уровня (регистрация через ScriptMgr/HookMgr "player.on_level_changed") ===
    // Вызывается из Player::LevelUpdate() после рассылки class_spells.
    // При oldLevel < 10 <= newLevel: автовыдача QUEST_PATH_CHOSEN (если ещё не
    // выполнен и нет активныой спеки) + SMSG_SPEC_SELECTION_OPEN.
    void OnLevelChanged(Player *player, uint8 oldLevel, uint8 newLevel);

    // === Выбор специализации ===
    // Единая точка входа для CMSG_CHOOSE_SPEC и Lua OnComplete квеста 1104.
    // Валидирует: уровень >= 10, SpecID принадлежит ClassID, спека ещё не выбрана
    // (перевыбор — только через тренера, вне этого пути, M2).
    // Возвращает false + reason (для SMSG_SPEC_ERROR, M1: пока лог в world-log).
    bool ChooseSpec(Player *player, uint8 specId, const char *&outReason);

    // Выдача способностей специализации (публичный API для Lua:
    // Player:GrantSpecAbilities(specId)). Очищает старые ауры спеки (если смена),
    // изучает SpellID_1..N из playercreateinfo_specs, шлёт SMSG_LEARNED_SPELLS_BATCH.
    // Возвращает список выученных SpellID (пусто — если данных нет).
    std::vector<uint32_t> GrantSpecAbilities(Player *player, uint8 specId);

    // Роль игрока для LFG по активной спеке (из кэша lfg_roles/spec primary_stat:
    // танк = STA-primary, хил = класс Priest, dps = остальные).
    uint8_t GetLfgRoleForPlayer(const Player *player) const;

    // Разрешён ли вход в очередь LFG: level>=MinLevel && (ачивка || активная спека).
    bool CanJoinLfg(const Player *player, uint16_t dungeonId, const char *&outReason) const;

private:
    struct SpecSpellsRow {
        uint8_t classId;
        uint8_t specId;
        std::vector<uint32_t> spellIds; // SpellID_1..4 (0 = слот пуст)
    };

    SpecQuestMgr() = default;

    const SpecSpellsRow *FindSpecSpells(uint8_t classId, uint8_t specId) const;
    void SendSpecSelectionOpen(Player *player);
    void SendLearnedSpellsBatch(Player *player, const std::vector<uint32_t> &spellIds);

    std::vector<SpecSpellsRow> _specSpells; // playercreateinfo_specs
};

// Регистрация сетевых обработчиков диспетчера (аналог RegisterGossipHandlers).
// Опкоды: CMSG_CHOOSE_SPEC 0x2024 (+ читатель SMSG_* внутри handler'ов).
void RegisterSpecQuestHandlers(class Network::OpcodeDispatcher &disp, SpecQuestMgr &mgr);

} // namespace Game
} // namespace DM
