// QuestStateMachine.h — Состояния квестов (GDD §8)
// Available→Accepted→Objective(n)→Complete→Rewarded
#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace DM {
namespace Quests {

// Состояния квеста
enum QuestState : uint8_t {
    QUEST_STATE_AVAILABLE = 0,   // Доступен для взятия
    QUEST_STATE_ACCEPTED = 1,    // Принят
    QUEST_STATE_OBJECTIVE = 2,   // Выполнение цели (n)
    QUEST_STATE_COMPLETE = 3,    // Все цели выполнены
    QUEST_STATE_REWARDED = 4,    // Награда получена
    QUEST_STATE_FAILED = 5,     // Провален
    QUEST_STATE_ABANDONED = 6,  // Отменён
};

// Тип цели
enum ObjectiveType : uint8_t {
    OBJ_KILL = 1,      // Убийство мобов
    OBJ_GATHER = 2,    // Сбор предметов/ресурсов
    OBJ_EXPLORE = 3,   // Посещение зоны
    OBJ_TALK = 4,      // Разговор с NPC
    OBJ_INSTANCE = 5,  // Прохождение инстанса
};

// Структура цели
struct QuestObjective {
    uint32_t targetEntry = 0;    // creature/GO/zone/NPC entry
    ObjectiveType type = OBJ_KILL;
    uint32_t requiredCount = 1;
    uint32_t currentCount = 0;
    std::string text;

    bool IsComplete() const {
        return currentCount >= requiredCount;
    }
};

// Состояние квеста игрока
struct PlayerQuestState {
    uint32_t questId = 0;
    QuestState state = QUEST_STATE_AVAILABLE;
    std::vector<QuestObjective> objectives;
    uint32_t timer = 0;          // таймер (для квестов с ограничением по времени)
    uint32_t acceptTime = 0;     // время принятия

    bool AllObjectivesComplete() const {
        for (const auto &obj : objectives) {
            if (!obj.IsComplete()) return false;
        }
        return !objectives.empty();
    }

    void UpdateObjective(uint32_t targetEntry, uint32_t count) {
        for (auto &obj : objectives) {
            if (obj.targetEntry == targetEntry) {
                obj.currentCount = std::min(obj.currentCount + count, obj.requiredCount);
            }
        }
        // Проверка перехода в Complete
        if (state == QUEST_STATE_OBJECTIVE && AllObjectivesComplete()) {
            state = QUEST_STATE_COMPLETE;
        }
    }
};

// Переходы состояний
inline bool CanTransition(QuestState from, QuestState to) {
    switch (from) {
        case QUEST_STATE_AVAILABLE:
            return to == QUEST_STATE_ACCEPTED;
        case QUEST_STATE_ACCEPTED:
            return to == QUEST_STATE_OBJECTIVE || to == QUEST_STATE_FAILED || to == QUEST_STATE_ABANDONED;
        case QUEST_STATE_OBJECTIVE:
            return to == QUEST_STATE_COMPLETE || to == QUEST_STATE_FAILED || to == QUEST_STATE_ABANDONED;
        case QUEST_STATE_COMPLETE:
            return to == QUEST_STATE_REWARDED || to == QUEST_STATE_ABANDONED;
        default:
            return false;
    }
}

// Обработка события принятия квеста
inline void OnQuestAccept(PlayerQuestState &state, uint32_t questId) {
    if (state.state != QUEST_STATE_AVAILABLE) return;
    state.questId = questId;
    state.state = QUEST_STATE_ACCEPTED;
    state.acceptTime = 0; // TODO: set server time
    if (!state.objectives.empty()) {
        state.state = QUEST_STATE_OBJECTIVE;
    }
}

// Обработка убийства моба
inline void OnMobKill(PlayerQuestState &state, uint32_t creatureEntry) {
    if (state.state != QUEST_STATE_OBJECTIVE) return;
    state.UpdateObjective(creatureEntry, 1);
}

// Обработка сбора предмета
inline void OnItemGather(PlayerQuestState &state, uint32_t itemEntry, uint32_t count) {
    if (state.state != QUEST_STATE_OBJECTIVE) return;
    state.UpdateObjective(itemEntry, count);
}

// Обработка посещения зоны
inline void OnZoneVisit(PlayerQuestState &state, uint32_t zoneId) {
    if (state.state != QUEST_STATE_OBJECTIVE) return;
    state.UpdateObjective(zoneId, 1);
}

// Обработка разговора с NPC
inline void OnNpcTalk(PlayerQuestState &state, uint32_t npcEntry) {
    if (state.state != QUEST_STATE_OBJECTIVE) return;
    state.UpdateObjective(npcEntry, 1);
}

// Обработка завершения квеста
inline void OnQuestComplete(PlayerQuestState &state) {
    if (state.state != QUEST_STATE_COMPLETE) return;
    state.state = QUEST_STATE_REWARDED;
}

// Обработка провала квеста
inline void OnQuestFail(PlayerQuestState &state) {
    if (state.state == QUEST_STATE_AVAILABLE || state.state == QUEST_STATE_REWARDED) return;
    state.state = QUEST_STATE_FAILED;
}

// Обработка отмены квеста
inline void OnQuestAbandon(PlayerQuestState &state) {
    if (state.state == QUEST_STATE_AVAILABLE || state.state == QUEST_STATE_REWARDED) return;
    state.state = QUEST_STATE_ABANDONED;
}

} // namespace Quests
} // namespace DM