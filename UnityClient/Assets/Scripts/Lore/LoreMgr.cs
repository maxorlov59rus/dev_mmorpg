using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Lore placeholder manager: serves static city/NPC/quest lists until the server drives them.
/// </summary>
public class LoreMgr : MonoBehaviour
{
    private readonly List<CityInfo> _cities = new List<CityInfo>
    {
        new CityInfo { id = 1, name_ru = "Ательгард" },
        new CityInfo { id = 2, name_ru = "Умбра" },
        new CityInfo { id = 3, name_ru = "Грейвхейвен" }
    };

    private readonly List<NpcInfo> _npcs = new List<NpcInfo>
    {
        new NpcInfo { id = 1, name_ru = "Высвященный Виксис", title_ru = "Глава Ордена Света", faction_id = 1 },
        new NpcInfo { id = 2, name_ru = "Леди Эльара", title_ru = "Хранительница храма", faction_id = 1 },
        new NpcInfo { id = 3, name_ru = "Великий Маг Таллон", title_ru = "Правитель Умбры", faction_id = 2 },
        new NpcInfo { id = 4, name_ru = "Друид Вильдан", title_ru = "Смотритель Грейвхейвена", faction_id = 3 }
    };

    private readonly List<QuestInfo> _quests = new List<QuestInfo>
    {
        new QuestInfo { id = 1001, title_ru = "Очищение окраин", description_ru = "Очистите заражённые земли Ательгарда.", level_min = 70, level_max = 75, reward_value = 500, reward_type = "gold" },
        new QuestInfo { id = 1002, title_ru = "Свет над руинами", description_ru = "Восстановите солнечный храм.", level_min = 75, level_max = 80, reward_value = 800, reward_type = "gold" }
    };

    public List<CityInfo> getAllCities()
    {
        return _cities;
    }

    public List<NpcInfo> getNpcsByCity(int cityId)
    {
        return _npcs;
    }

    public List<QuestInfo> getQuestsByCity(int cityId)
    {
        return _quests;
    }

    public NpcInfo getNpcById(int npcId)
    {
        return _npcs.Find(n => n.id == npcId);
    }

    public List<QuestInfo> getQuestsByNpc(int npcId)
    {
        return _quests;
    }

    public List<QuestInfo> getAvailableQuests(int playerLevel, int factionId)
    {
        return _quests.FindAll(q => q.level_min <= playerLevel + 5);
    }
}
