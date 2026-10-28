// =====================================================================
// «Чёрная Материя» — PlayerClassMgr.cpp
// Кэширование playercreateinfo_classlevelstats / class_specs / class_spells /
// class_restrictions (dm-server/src/game/Classes, docs/02 §5).
//
// Стиль загрузки — TRC SQLStorage: один SELECT на boot, вектор-строки +
// композитные маски для O(1)-проверок. Никаких float — все статы целые.
// =====================================================================
#include "PlayerClassMgr.h"

#include <algorithm>
#include <array>

// Интеграционные заглушки: реальные заголовки появятся вместе с ядром
// (docs/02 §Структура: shared/Database, game/Entities/Player.h).
// Методы, помеченные // TODO(KERNEL), требуют связывания с Player API.
namespace DM {

// ------------------------------------------------------------------ //
// Синглтон и загрузка                                                //
// ------------------------------------------------------------------ //

PlayerClassMgr &PlayerClassMgr::instance() {
    static PlayerClassMgr s;
    return s;
}

bool PlayerClassMgr::loadFromDb() {
    _restrictions.clear();
    _statsKeys.clear();
    _statsRows.clear();
    _spells.clear();
    _specs.clear();
    std::fill(std::begin(_classRaceMask), std::end(_classRaceMask), uint8{0});

    // NOTE(KERNEL): CharacterDatabase->Query через общий QueryResult池 (TRC-style).
    // Здесь — контракт запросов; парсер строк вынесен в SqlRowReader (shared/Database).
    //
    // 1) class_restrictions: ClassID -> ForbiddenRaceID (+ опц. required faction)
    //    SELECT class_id, forbidden_race_id, required_faction FROM class_restrictions;
    // 2) playercreateinfo_classlevelstats: стартовые статы на уровень
    //    SELECT race, class, level, strength, agility, stamina, intellect, spirit,
    //           hp_max, power_max, armor, spell_power
    //      FROM playercreateinfo_classlevelstats ORDER BY race, class, level;
    // 3) class_spells: автоизучение
    //    SELECT class_id, level, spell_id FROM class_spells ORDER BY class_id, level;
    // 4) class_specs: специализации
    //    SELECT spec_id, class_id, name_ru, description_ru, primary_stat
    //      FROM class_specs;

    // ---- Демо-сид (M0, пока нет подключения к MySQL-пулу; сверяется с
    // sql/upgrades/Create_Class_Database.sql — golden-data тест CI сравнивает
    // обе стороны и не даёт им разойтись): --------------------------------
    _restrictions = {
        {CLASS_WARLOCK, RACE_HUMAN, 2},       // Чернокнижник закрыт людям, только Круг Поглощения
        {CLASS_WARLOCK, RACE_LUMENAR, 2},     // ...и светорождённым
        {CLASS_PRIEST, RACE_ASHEN, 0},        // Жрец не играет за Орден Пепла
        {CLASS_PRIEST, RACE_HOLLOWED, 0},
    };

    auto addStats = [&](uint8 race, uint8 cls, uint8 lvl, StartStats st) {
        _statsKeys.push_back({race, cls, lvl});
        _statsRows.push_back(st);
    };
    // Воин (плита, сила) — люди/пепельные
    addStats(RACE_HUMAN, CLASS_WARRIOR, 1, {/*str*/ 22, /*agi*/ 12, /*sta*/ 20, /*int*/ 6, /*spi*/ 8, /*hp*/ 120, /*pow(rage cap)*/ 100, /*armor*/ 90, /*sp*/ 0});
    addStats(RACE_ASHEN, CLASS_WARRIOR, 1, {24, 11, 21, 5, 7, 126, 100, 92, 0});
    addStats(RACE_HUMAN, CLASS_WARRIOR, 5, {34, 18, 31, 9, 12, 210, 100, 140, 0});
    // Маг (ткань, интеллект)
    addStats(RACE_HUMAN, CLASS_MAGE, 1, {8, 14, 12, 24, 18, 78, 160, 30, 40});
    addStats(RACE_LUMENAR, CLASS_MAGE, 1, {7, 15, 11, 27, 20, 82, 175, 28, 46});
    addStats(RACE_LUMENAR, CLASS_MAGE, 5, {12, 21, 17, 38, 28, 132, 240, 44, 74});
    // Разбойник (кожа, ловкость)
    addStats(RACE_ASHEN, CLASS_ROGUE, 1, {12, 24, 15, 8, 9, 96, 100, 55, 0});
    addStats(RACE_THRESHOLDKIN, CLASS_ROGUE, 1, {11, 26, 14, 9, 10, 94, 100, 52, 0});
    addStats(RACE_ASHEN, CLASS_ROGUE, 5, {18, 38, 22, 12, 14, 158, 100, 84, 0});

    _spells = {
        {CLASS_WARRIOR, 1, 100 /*Удар (Heroic Strike)*/},
        {CLASS_WARRIOR, 1, 78 /*Уклонение*/},
        {CLASS_WARRIOR, 4, 116 /*Рывок (Charge)*/},
        {CLASS_MAGE, 1, 133 /*Огненная стрела*/},
        {CLASS_MAGE, 1, 44008 /*Полёт камня (binding)*/},
        {CLASS_MAGE, 4, 122 /*Ледяная буря*/},
        {CLASS_ROGUE, 1, 1752 /*Удар в спину*/},
        {CLASS_ROGUE, 1, 2983 /*Скрытность*/},
        {CLASS_ROGUE, 4, 53 /*Полоса удушения*/},
    };

    _specs = {
        {SPEC_WAR_SIEGE, CLASS_WARRIOR, "Осадный", "Сила разрушения: замедляющие удары по щитам, бонус к Strength-кастам.", 1},
        {SPEC_WAR_VANGUARD, CLASS_WARRIOR, "Авангард", "Стена щитов: угроза, блок, временные HP от Порчи.", 3},
        {SPEC_WAR_DUELIST, CLASS_WARRIOR, "Дуэлянт", "Клинок и стойка: крит от Agility, контрудары.", 2},
        {SPEC_MAGE_FROST, CLASS_MAGE, "Мороз", "Ледяные кристаллы Niagara: заморозка, каст сквозь контроль.", 4},
        {SPEC_MAGE_SEEPAGE, CLASS_MAGE, "Истечения", "Школа Чёрной Материи: DoT-вены, рост урона с Corruption.", 4},
        {SPEC_MAGE_LIGHT, CLASS_MAGE, "Свет", "Священные мотыльки: лечение союзников, изгнание Полых.", 4},
        {SPEC_ROG_SHADOW, CLASS_ROGUE, "Тень", "Afterimage при спринте: урон из стелса, энергия за убийства.", 2},
        {SPEC_ROG_ALTRUIST, CLASS_ROGUE, "Послушник", "Яды и расходники: отравления, антидоты, торговля на чёрном рынке.", 2},
        {SPEC_ROG_GATE, CLASS_ROGUE, "Порожник", "Портальные кувырки: рывки через Истечения ЧМ.", 2},
    };

    // Строим inverse-маску: по умолчанию всё разрешено, restrictions выключают биты.
    for (uint8 c = 1; c < CLASS_COUNT; ++c)
        _classRaceMask[c] = 0x3F; // bits 1..5 (RACE_COUNT==6 → маска 0b111110)
    for (const auto &r : _restrictions)
        if (r.classId < CLASS_COUNT && r.forbiddenRaceId < RACE_COUNT)
            _classRaceMask[r.classId] &= static_cast<uint8>(~(1u << r.forbiddenRaceId));
    // ---------------------------------------------------------------------- //

    return !_statsKeys.empty() && !_specs.empty();
}

// ------------------------------------------------------------------ //
// Валидация                                                          //
// ------------------------------------------------------------------ //

bool PlayerClassMgr::CanCreateClass(uint8 raceId, uint8 classId) const {
    if (raceId == 0 || raceId >= RACE_COUNT)
        return false;
    if (classId == 0 || classId >= CLASS_COUNT)
        return false;
    return (_classRaceMask[classId] >> raceId) & 1u;
}

bool PlayerClassMgr::SpecBelongsToClass(uint8 specId, uint8 classId) const {
    const ClassSpecInfo *s = FindSpec(specId);
    return s && s->classId == classId;
}

ClassCheckResult PlayerClassMgr::ValidateCreation(uint8 raceId, uint8 classId, uint8 factionId, uint8 specId) const {
    if (raceId == 0 || raceId >= RACE_COUNT)
        return ClassCheckResult::InvalidRace;
    if (classId == 0 || classId >= CLASS_COUNT)
        return ClassCheckResult::InvalidClass;
    if (!CanCreateClass(raceId, classId))
        return ClassCheckResult::RaceClassForbidden;

    // Faction-bound классы: строки restrictions с required_faction != 0.
    for (const auto &r : _restrictions)
        if (r.classId == classId && r.requiredFaction != 0 && factionId != 0 && factionId != r.requiredFaction)
            return ClassCheckResult::FactionMismatch;

    // SpecID обязателен для классов со спеками (воин/маг/разбойник), M0.
    bool classHasSpecs = std::any_of(_specs.begin(), _specs.end(),
                                     [&](const ClassSpecInfo &s) { return s.classId == classId; });
    if (classHasSpecs && !SpecBelongsToClass(specId, classId))
        return ClassCheckResult::InvalidSpec;

    StartStats tmp;
    if (!GetStartStats(raceId, classId, 1, tmp))
        return ClassCheckResult::LevelStatsMissing;

    return ClassCheckResult::Ok;
}

std::vector<uint8> PlayerClassMgr::GetAllowedClassesForRace(uint8 raceId) const {
    std::vector<uint8> out;
    for (uint8 c = 1; c < CLASS_COUNT; ++c)
        if (CanCreateClass(raceId, c))
            out.push_back(c);
    return out;
}

std::vector<uint8> PlayerClassMgr::GetAllowedRacesForClass(uint8 classId) const {
    std::vector<uint8> out;
    if (classId == 0 || classId >= CLASS_COUNT)
        return out;
    for (uint8 r = 1; r < RACE_COUNT; ++r)
        if (CanCreateClass(r, classId))
            out.push_back(r);
    return out;
}

// ------------------------------------------------------------------ //
// Стартовые статы                                                    //
// ------------------------------------------------------------------ //

bool PlayerClassMgr::GetStartStats(uint8 raceId, uint8 classId, uint8 level, StartStats &out) const {
    // TRC хранит только 1-й уровень + пересчёт на уровень; здесь — прямые строки
    // на каждый запрошенный уровень (design-таблица заполняет SQL-файл).
    for (size_t i = 0; i < _statsKeys.size(); ++i) {
        const StatsKey &k = _statsKeys[i];
        if (k.race == raceId && k.classId == classId && k.level == level) {
            out = _statsRows[i];
            return true;
        }
    }
    return false;
}

PowerType PlayerClassMgr::GetPowerType(uint8 classId) const {
    switch (classId) {
    case CLASS_WARRIOR:
        return PowerType::Rage;
    case CLASS_ROGUE:
    case CLASS_HUNTER:
        return PowerType::Energy;
    case CLASS_PRIEST:
    case CLASS_MAGE:
    case CLASS_WARLOCK:
        return PowerType::Mana;
    default:
        return PowerType::None;
    }
}

void PlayerClassMgr::ApplyStartStats(Player *player) const {
    if (!player)
        return;
    // TODO(KERNEL): сигнатуры Player API появятся с dm-server/src/game/Entities:
    //   uint8 GetRace() const; uint8 GetClass() const; uint8 GetLevel() const;
    //   void SetBaseStat(Stat, uint32); void SetMaxPower(PowerType, uint32);
    //   void SetDisplayHealth(uint32); void DirtyCharStats(); // -> UPDATE char_stats
    StartStats st{};
    // const uint8 race = player->GetRace(), cls = player->GetClass(), lvl = player->GetLevel();
    const uint8 race = 1, cls = 1, lvl = 1; // placeholder до линковки с Entities/Player.h
    if (!GetStartStats(race, cls, lvl, st))
        return; // hard-fail лога: missing row в playercreateinfo_classlevelstats

    // player->SetBaseStat(STAT_STRENGTH, st.strength); ... (5 вызовов)
    // player->SetMaxPower(GetPowerType(cls), st.powerMax);
    (void)st;
    // TODO(KERNEL): BuildSMSG_InitializeStats(player) -> SMSG_INITIALIZE_STATS 0x0090
}

// ------------------------------------------------------------------ //
// Способности                                                        //
// ------------------------------------------------------------------ //

std::vector<uint32> PlayerClassMgr::LearnDefaultSpells(Player *player) const {
    std::vector<uint32> learned;
    if (!player)
        return learned;
    // TODO(KERNEL): как в ApplyStartStats — заменить на player->GetClass()/GetLevel().
    const uint8 cls = 1, lvl = 1;
    for (const auto &row : _spells) {
        if (row.classId != cls || row.level > lvl)
            continue;
        // if (player->HasSpell(row.spellId)) continue;  // char_spells already loaded
        // player->AddSpell(row.spellId, /*src=*/SPELL_SRC_CLASS_DEFAULT); // INSERT char_spells
        learned.push_back(row.spellId);
    }
    // TODO(KERNEL): отправить пачку SMSG_LEARN_SPELL (0x0091) — по одному пакету
    // на спелл (additive rule: клиент рисует иконки в action bar сразу).
    return learned;
}

std::vector<uint32> PlayerClassMgr::LearnSpellsForLevel(Player *player, uint8 newLevel) const {
    std::vector<uint32> learned;
    if (!player)
        return learned;
    const uint8 cls = 1; // TODO(KERNEL): player->GetClass()
    for (const auto &row : _spells)
        if (row.classId == cls && row.level == newLevel)
            learned.push_back(row.spellId);
    return learned;
}

// ------------------------------------------------------------------ //
// Специализации                                                      //
// ------------------------------------------------------------------ //

const ClassSpecInfo *PlayerClassMgr::FindSpec(uint8 specId) const {
    for (const auto &s : _specs)
        if (s.specId == specId)
            return &s;
    return nullptr;
}

std::vector<ClassSpecInfo> PlayerClassMgr::GetSpecsForClass(uint8 classId) const {
    std::vector<ClassSpecInfo> out;
    for (const auto &s : _specs)
        if (s.classId == classId)
            out.push_back(s);
    return out;
}

// ------------------------------------------------------------------ //
// Хуки ScriptMgr                                                     //
// ------------------------------------------------------------------ //

namespace ClassHooks {

class CharacterHandler; // fwd — реальный тип появится с handlers/CharacterCreateHandler.h

void OnValidateCharacter(CharacterHandler &handler, uint8 race, uint8 classId, uint8 faction, uint8 spec) {
    // Вызывается из CharacterCreateHandler ДО записи в БД (docs charcreate §2).
    // При != Ok handler отвечает SMSG_CHAR_CREATE_ERROR{reason}.
    ClassCheckResult res = PlayerClassMgr::instance().ValidateCreation(race, classId, faction, spec);
    if (res != ClassCheckResult::Ok) {
        // TODO(KERNEL): handler.Reject(ECharCreateError::ClassForbidden /* маппинг res */);
        (void)handler;
    }
}

void OnFirstLogin(Player *player) {
    PlayerClassMgr::instance().ApplyStartStats(player);
    std::vector<uint32> spells = PlayerClassMgr::instance().LearnDefaultSpells(player);
    (void)spells; // TODO(KERNEL): flush SMSG_LEARN_SPELL batch
}

void OnLevelChanged(Player *player, uint8 /*oldLevel*/, uint8 newLevel) {
    std::vector<uint32> spells = PlayerClassMgr::instance().LearnSpellsForLevel(player, newLevel);
    (void)spells; // TODO(KERNEL): flush SMSG_LEARN_SPELL + SMSG_LEVEL_UP
}

} // namespace ClassHooks

} // namespace DM
