// =====================================================================
// «Чёрная Материя» — PlayerClassMgr (аналог TrinityCore PlayerCreate +
// playercreateinfo_* таблицы, docs/02 §5).
//
// Ответственности:
//  1) Валидация связки Race×Class при CMSG_CHAR_CREATE (class_restrictions).
//  2) Выдача стартовых статов из playercreateinfo_classlevelstats.
//  3) Автоизучение классовых способностей по уровням (class_spells) при
//     первом входе в мир и при level-up.
//  4) Валидация специализации (SpecID) при создании и смене спеки.
//
// Данные грузятся из dm_characters в кэш синглтона на старте worldserver
// (паттерн HotSwapableCache<> из TRC); Lua может вызывать те же методы
// через HookMgr (RegisterPlayerEvent / ScriptMgr hooks).
//
// Wire-протокол: proto/opcodes.defs (SMSG_INITIALIZE_STATS 0x0090,
// SMSG_LEARN_SPELL 0x0091), клиентский enum — MMOProtocol.h.
// =====================================================================
#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

// Сокращённые алиасы (как в TRC Common.h): uint8/16/32 = std-эквиваленты.
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

namespace DM {

class Player;

// ---- Канонические ID (совпадают с dbc и DataTable клиента) ----
enum RaceId : uint8_t {
    RACE_HUMAN = 1,        // Альянс Света (люди Рассвета)
    RACE_LUMENAR = 2,      // Альянс Света (светорождённые)
    RACE_ASHEN = 3,        // Орден Пепла (пепельные)
    RACE_HOLLOWED = 4,     // Орден Пепла (Полые, частично изменённые ЧМ)
    RACE_THRESHOLDKIN = 5, // Гильдия Порога (полукровки порога)
    RACE_COUNT = 6,
};

enum ClassId : uint8_t {
    CLASS_WARRIOR = 1, // Воин        — primary: Strength
    CLASS_PRIEST = 2,  // Жрец        — primary: Intellect
    CLASS_MAGE = 3,    // Маг         — primary: Intellect
    CLASS_ROGUE = 4,   // Разбойник   — primary: Agility
    CLASS_HUNTER = 5,  // Охотник     — primary: Agility
    CLASS_WARLOCK = 6, // Чернокнижник— primary: Intellect (закрыт для Альянса)
    CLASS_COUNT = 7,
};

enum class PowerType : uint8_t {
    None = 0,
    Mana = 1,   // Жрец, Маг, Чернокнижник
    Rage = 2,   // Воин
    Energy = 3, // Разбойник, Охотник (pet — mana)
};

enum SpecId : uint8_t {
    SPEC_NONE = 0,
    // Воин
    SPEC_WAR_SIEGE = 1,    // Осадный (Сила, plate, motion-blur удары)
    SPEC_WAR_VANGUARD = 2, // Авангард (танк/выживаемость)
    SPEC_WAR_DUELIST = 3,  // Дуэлянт (гибкость)
    // Маг
    SPEC_MAGE_FROST = 4,   // Мороз (ледяные кристаллы Niagara)
    SPEC_MAGE_SEEPAGE = 5, // Истечения (Школа Чёрной Материи)
    SPEC_MAGE_LIGHT = 6,   // Свет
    // Разбойник
    SPEC_ROG_SHADOW = 7,   // Тень (afterimage при спринте)
    SPEC_ROG_ALTRUIST = 8, // Послушник (яды/расходники)
    SPEC_ROG_GATE = 9,     // Порожник (рывки, портальные кувырки)
    SPEC_COUNT = 10,
};

// Стартовая структура статов (raw, до баффов/Gear). Все значения — целые,
// детерминированные (docs/02 §6: никаких float в серверных формулах).
struct StartStats {
    uint32 strength = 0;
    uint32 agility = 0;
    uint32 stamina = 0;
    uint32 intellect = 0;
    uint32 spirit = 0;
    uint32 hpMax = 0;    // фикс. ед. (Q16 внутри Combat, здесь — display int)
    uint32 powerMax = 0; // Mana/Rage/Energy cap
    uint32 armor = 0;
    uint32 spellPower = 0;
};

// Строка class_specs (для UI и выбора спеки).
struct ClassSpecInfo {
    uint8 specId = SPEC_NONE;
    uint8 classId = CLASS_WARRIOR;
    std::string_view nameRu;
    std::string_view descriptionRu;
    uint8 primaryStat = 0; // 1=str 2=agi 3=sta 4=int 5=spi
};

// Причина отказа создания (зеркало ECharCreateError клиента, MMOProtocol.h).
enum class ClassCheckResult : uint8_t {
    Ok = 0,
    InvalidRace = 1,
    InvalidClass = 2,       // class == 0 или >= CLASS_COUNT
    RaceClassForbidden = 3, // class_restrictions (например Warlock×Human)
    FactionMismatch = 4,    // класс требует фракцию (Warlock — только Круг)
    InvalidSpec = 5,        // SpecID не принадлежит ClassID
    LevelStatsMissing = 6,  // нет строки playercreateinfo_classlevelstats
};

constexpr std::string_view ToString(ClassCheckResult r) {
    switch (r) {
    case ClassCheckResult::Ok:
        return "ok";
    case ClassCheckResult::InvalidRace:
        return "invalid_race";
    case ClassCheckResult::InvalidClass:
        return "invalid_class";
    case ClassCheckResult::RaceClassForbidden:
        return "race_class_forbidden";
    case ClassCheckResult::FactionMismatch:
        return "faction_mismatch";
    case ClassCheckResult::InvalidSpec:
        return "invalid_spec";
    case ClassCheckResult::LevelStatsMissing:
        return "level_stats_missing";
    }
    return "unknown";
}

// ---- Синглтон менеджера классов ----
class PlayerClassMgr {
public:
    static PlayerClassMgr &instance();

    // Загрузка кэшей из БД (worldserver boot / hot-reload по команде GM).
    bool loadFromDb();

    // === Артефакт 1: валидация ===
    // Проверка разрешённости связки race×class (класс-таблица + restrictions).
    bool CanCreateClass(uint8 raceId, uint8 classId) const;
    // Полный предпроверочный прогон для CMSG_CHAR_CREATE (включая spec/faction).
    ClassCheckResult ValidateCreation(uint8 raceId, uint8 classId, uint8 factionId,
                                      uint8 specId) const;
    // Список классов, доступных расе (для UI_CharacterCreate: блокировка кнопок).
    std::vector<uint8> GetAllowedClassesForRace(uint8 raceId) const;
    // Список рас, доступных классу (обратный фильтр при выборе класса первым).
    std::vector<uint8> GetAllowedRacesForClass(uint8 classId) const;
    bool SpecBelongsToClass(uint8 specId, uint8 classId) const;

    // === Стартовые параметры ===
    // Статы уровня `level` для race×class (БД-строка, кэш). false — если нет.
    bool GetStartStats(uint8 raceId, uint8 classId, uint8 level, StartStats &out) const;
    // Применяет базовые статы к персу (пишет char_stats + memory-cache Player).
    void ApplyStartStats(Player *player) const;
    PowerType GetPowerType(uint8 classId) const;

    // === Способности ===
    // Изучение всех class_spells c.Level <= player->GetLevel(), отсутствующих
    // в char_spells. Возвращает список выученных SpellID (для SMSG_LEARN_SPELL).
    std::vector<uint32> LearnDefaultSpells(Player *player) const;
    // Вызывается из Player::LevelUpdate(): выдаёт спеллы нового уровня.
    std::vector<uint32> LearnSpellsForLevel(Player *player, uint8 newLevel) const;

    // === Специализации ===
    const ClassSpecInfo *FindSpec(uint8 specId) const;
    std::vector<ClassSpecInfo> GetSpecsForClass(uint8 classId) const;

private:
    PlayerClassMgr() = default;

    struct RestrictionEntry {
        uint8 classId;
        uint8 forbiddenRaceId;
        uint8 requiredFaction; // 0 = без привязки к фракции
    };
    struct StatsKey {
        uint8 race;
        uint8 classId;
        uint8 level;
    };
    struct SpellsRow {
        uint8 classId;
        uint8 level;
        uint32 spellId;
    };

    std::vector<RestrictionEntry> _restrictions;
    std::vector<StatsKey> _statsKeys;
    std::vector<StartStats> _statsRows; // параллельно _statsKeys
    std::vector<SpellsRow> _spells;
    std::vector<ClassSpecInfo> _specs;

    // Маска доступности class->bit(race), заполняется из restrictions (inverse).
    uint8 _classRaceMask[CLASS_COUNT] = {};
};

// Хуки ScriptMgr (Lua-видимые имена): OnValidateCharacter / OnPlayerLogin.
// Реализация вызывает singleton; сигнатуры совпадают с Eluna-style реестром
// dm-server/scripts/lua/LuaRacialSystem.lua (общий паттерн подклассов).
namespace ClassHooks {
void OnValidateCharacter(class CharacterHandler &handler, uint8 race, uint8 classId, uint8 faction,
                         uint8 spec);
void OnFirstLogin(Player *player); // ApplyStartStats + LearnDefaultSpells + SMSG_INITIALIZE_STATS
void OnLevelChanged(Player *player, uint8 oldLevel, uint8 newLevel);
} // namespace ClassHooks

} // namespace DM
