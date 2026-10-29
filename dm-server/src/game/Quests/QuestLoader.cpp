// QuestLoader.cpp — реализация реестра квестов и проверки целей
#include "QuestLoader.h"

#include <algorithm>

// TODO(KERNEL): подключить реальный SqlSession (dm-server/src/database).
// Контракт запросов зафиксирован здесь и совпадает со схемой Insert_Quests_6_15.sql:
//   SELECT ID,Title,LevelMin,LevelMax,ZoneID,NextQuestId,PrevQuestId,QuestType,
//          SpecialFlags,RewardReputationFaction1,RewardReputationAmount1,RewardMoney
//   FROM quest_template WHERE ID BETWEEN 1001 AND 1015;
//   SELECT ID,Type,ObjIndex,TargetEntry,Count,ObjectiveText FROM quest_objective;
//   SELECT npc_entry,quest_id,quest_direction FROM npc_quest_giver;

namespace dm {

QuestLoader &QuestLoader::Instance() {
    static QuestLoader inst;
    return inst;
}

bool QuestLoader::LoadFromDatabase(SqlSession * /*session*/) {
    // Пустой прогон без БД: оставляем детерминированный снимок для unit-теста ядра.
    if (!m_quests.empty())
        return true;

    auto add = [this](uint32_t id, const char *title, uint8_t lo, uint8_t hi, uint32_t zone,
                      uint32_t nextQ, uint32_t prevQ, uint8_t type, uint8_t flags,
                      std::vector<QuestObjective> objs) {
        QuestData q;
        q.Id = id;
        q.Title = title;
        q.MinLevel = lo;
        q.MaxLevel = hi;
        q.ZoneId = zone;
        q.NextQuestId = nextQ;
        q.PrevQuestId = prevQ;
        q.QuestType = type;
        q.SpecialFlags = flags;
        q.Objectives = std::move(objs);
        m_quests.emplace(id, std::move(q));
    };

    // Цепочка 1 (1-10): 1001->1002->1003->1004->1005
    add(1001, "Первые испытания", 6, 8, 2, 1002, 0, 1, 0,
        {{1001, QuestObjectiveType::KillCredit, 0, 50001, 5, "Убить слизней"}});
    add(1002, "Доставка донесения", 7, 9, 2, 1003, 1001, 1, 0,
        {{1002, QuestObjectiveType::TalkNpc, 0, 70011, 1, "Командор Велан"}});
    add(1003, "Снабжение фронта", 8, 10, 2, 1004, 1002, 1, 0,
        {{1003, QuestObjectiveType::ItemGather, 0, 60001, 10, "Медная руда"}});
    add(1004, "Путь избранного", 10, 10, 3, 1005, 1003, 2, 0x1,
        {{1004, QuestObjectiveType::TalkNpc, 0, 70012, 1, "Наставник Ирана"}});
    add(1005, "Проверка боем", 10, 12, 20, 1009, 1004, 2, 0x2,
        {{1005, QuestObjectiveType::ExploreArea, 0, 20, 1, "Пещеры Испытаний"}});

    // Цепочка 2 (10-15): 1009->1010->1011->1012
    add(1009, "Зачистка аванпоста", 10, 12, 4, 1010, 1005, 1, 0,
        {{1009, QuestObjectiveType::KillCredit, 0, 50003, 8, "Бандиты аванпоста"}});
    add(1010, "Сбитие гарпий", 11, 13, 4, 1011, 1009, 1, 0,
        {{1010, QuestObjectiveType::KillCredit, 0, 50004, 6, "Гарпии"}});
    add(1011, "Разведка Крепости Пепла", 12, 14, 5, 1012, 1010, 1, 0,
        {{1011, QuestObjectiveType::ExploreArea, 0, 5, 1, "Крепость Пепла"}});
    add(1012, "Штурм крепости", 13, 15, 5, 0, 1011, 1, 0,
        {{1012, QuestObjectiveType::KillCredit, 0, 50005, 1, "Гаррош Пепельный"}});

    // Бонусные
    add(1013, "Охота на волков", 6, 10, 2, 0, 0, 1, 0,
        {{1013, QuestObjectiveType::KillCredit, 0, 50002, 6, "Волки"}});
    add(1014, "Медь для кузнеца", 7, 12, 2, 0, 0, 1, 0,
        {{1014, QuestObjectiveType::ItemGather, 0, 60001, 8, "Медная руда"}});
    add(1015, "Травница просит", 6, 11, 2, 0, 0, 1, 0,
        {{1015, QuestObjectiveType::ItemGather, 0, 60002, 6, "Лунотрава"}});

    // Привязки NPC (npc_quest_giver)
    struct Link {
        uint32_t Npc;
        uint32_t Quest;
    };
    static constexpr Link kLinks[] = {
        {70011, 1001}, {70011, 1002}, {70011, 1009}, {70011, 1013}, {70012, 1004}, {70012, 1005},
        {70013, 1015}, {70014, 1014}, {70014, 1003}, {70015, 1010}, {70015, 1011}, {70015, 1012},
    };
    for (const auto &l : kLinks)
        m_npcToQuests[l.Npc].push_back(l.Quest);

    return true;
}

const QuestData *QuestLoader::Find(uint32_t questId) const {
    auto it = m_quests.find(questId);
    return it == m_quests.end() ? nullptr : &it->second;
}

const std::vector<uint32_t> &QuestLoader::QuestIdsForNpc(uint32_t npcEntry) const {
    static const std::vector<uint32_t> kEmpty;
    auto it = m_npcToQuests.find(npcEntry);
    return it == m_npcToQuests.end() ? kEmpty : it->second;
}

uint32_t QuestLoader::ChainNext(uint32_t questId) const {
    const QuestData *q = Find(questId);
    return q ? q->NextQuestId : 0u;
}

bool QuestLoader::IsQuestObjectiveComplete(const PlayerQuestState &state, uint32_t questId) const {
    const QuestData *q = Find(questId);
    if (!q || q->Id != state.QuestId)
        return false;
    for (const auto &obj : q->Objectives) {
        if (obj.Index >= state.CountsPerObjective.size())
            return false;
        if (state.CountsPerObjective[obj.Index] < obj.RequiredCount)
            return false;
    }
    return true;
}

} // namespace dm
