// ResurrectionSystem.h — Система воскрешения персонажей
// GDD §3: Death: resurrection window 60s (ally res / self-res item), иначе ghost-run к кладбищу
#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace DM {
namespace Game {

// Типы воскрешения
enum ResurrectionType : uint8_t {
    RESURRECT_NONE = 0,
    RESURRECT_ALLY_SPELL = 1,      // Воскрешение союзником (заклинание)
    RESURRECT_SELF_ITEM = 2,       // Самовоскрешение (предмет)
    RESURRECT_SPIRIT_HEALER = 3,   // Духовный целитель (NPC)
    RESURRECT_CORPSE_RUN = 4,      // Бег к телу (ghost-run)
    RESURRECT_BATTLEGROUND = 5,    // Воскрешение на БГ
    RESURRECT_INSTANCE = 6,        // Воскрешение в инстансе
};

// Состояние смерти игрока
enum DeathState : uint8_t {
    DEATH_STATE_ALIVE = 0,
    DEATH_STATE_DYING = 1,         // Анимация смерти (0-3 сек)
    DEATH_STATE_DEAD = 2,          // Мёртв, окно воскрешения 60 сек
    DEATH_STATE_GHOST = 3,         // Дух, бег к телу/кладбищу
    DEATH_STATE_RESURRECTING = 4,  // В процессе воскрешения
};

// Локация кладбища
struct Graveyard {
    uint32_t id = 0;
    uint32_t mapId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    std::string name;
    uint32_t zoneId = 0;
    bool isInstanceGraveyard = false; // кладбище инстанса
};

// NPC духовного целителя
struct SpiritHealer {
    uint32_t npcEntry = 0;
    uint32_t graveyardId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    uint32_t mapId = 0;
    std::string name;
    bool isActive = true;
};

// Состояние смерти игрока
struct PlayerDeathState {
    DeathState state = DEATH_STATE_ALIVE;
    uint64_t deathTime = 0;          // время смерти (server tick)
    uint64_t resurrectWindowEnd = 0; // конец окна воскрешения (60 сек)
    uint32_t corpseGuid = 0;         // GUID трупа
    float corpseX = 0.0f;
    float corpseY = 0.0f;
    float corpseZ = 0.0f;
    uint32_t corpseMapId = 0;
    uint32_t graveyardId = 0;        // ближайшее кладбище
    uint32_t spiritHealerId = 0;     // ближайший духовный целитель
    uint8_t durabilityLoss = 0;      // потеря прочности (%)
    bool inInstance = false;         // смерть в инстансе
    bool corruptionGain = false;     // получена ли порча (смерть в Истечении)
};

// Параметры воскрешения
struct ResurrectionParams {
    ResurrectionType type = RESURRECT_NONE;
    uint32_t spellId = 0;           // ID заклинания воскрешения
    uint32_t itemId = 0;            // ID предмета самовоскрешения
    uint32_t healerId = 0;          // ID духовного целителя
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    uint32_t mapId = 0;
    bool hasDurabilityLoss = true;
    bool hasCorruptionGain = false;
    uint32_t sicknessSpellId = 0;   // болезнь воскрешения (если есть)
};

// Константы
constexpr uint32_t RESURRECT_WINDOW_SEC = 60;       // окно воскрешения 60 сек
constexpr uint8_t DURABILITY_LOSS_PCT = 10;          // потеря прочности 10%
constexpr uint32_t CORPSE_DECAY_SEC = 3600;          // труп исчезает через 1 час
constexpr uint32_t GHOST_SPEED_MULTIPLIER = 2;       // скорость духа x2

// Функции
class ResurrectionMgr {
public:
    static ResurrectionMgr &Instance();

    // Обработка смерти игрока
    void OnPlayerDeath(uint64_t playerGuid, uint32_t mapId, float x, float y, float z, bool inInstance, bool inSeepage);

    // Проверка возможности воскрешения
    bool CanResurrect(uint64_t playerGuid, ResurrectionType type) const;

    // Воскрешение союзником
    bool ResurrectByAlly(uint64_t playerGuid, uint64_t casterGuid, uint32_t spellId);

    // Самовоскрешение (предмет)
    bool ResurrectByItem(uint64_t playerGuid, uint32_t itemId);

    // Воскрешение духовным целителем
    bool ResurrectBySpiritHealer(uint64_t playerGuid, uint32_t healerId);

    // Ghost-run: бег к телу
    bool StartGhostRun(uint64_t playerGuid);

    // Воскрешение у тела
    bool ResurrectAtCorpse(uint64_t playerGuid);

    // Воскрешение в инстансе
    bool ResurrectInInstance(uint64_t playerGuid);

    // Поиск ближайшего кладбища
    uint32_t FindNearestGraveyard(uint32_t mapId, float x, float y, float z) const;

    // Поиск ближайшего духовного целителя
    uint32_t FindNearestSpiritHealer(uint32_t mapId, float x, float y, float z) const;

    // Обновление состояния (таймеры, окна)
    void Update(uint32_t deltaMs);

    // Проверка: в окне воскрешения?
    bool IsInResurrectWindow(uint64_t playerGuid) const;

    // Получение состояния смерти
    const PlayerDeathState* GetDeathState(uint64_t playerGuid) const;

    // Очистка состояния (при воскрешении)
    void ClearDeathState(uint64_t playerGuid);

private:
    ResurrectionMgr() = default;
    std::unordered_map<uint64_t, PlayerDeathState> m_deathStates;
    std::vector<Graveyard> m_graveyards;
    std::vector<SpiritHealer> m_spiritHealers;

    void LoadGraveyards();
    void LoadSpiritHealers();
    void SendDeathPacket(uint64_t playerGuid);
    void SendResurrectPacket(uint64_t playerGuid, const ResurrectionParams &params);
    void ApplyDurabilityLoss(uint64_t playerGuid, uint8_t lossPct);
    void ApplyCorruptionGain(uint64_t playerGuid);
    void RemoveCorpse(uint64_t playerGuid);
};

} // namespace Game
} // namespace DM