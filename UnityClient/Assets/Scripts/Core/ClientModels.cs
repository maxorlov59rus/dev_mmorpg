using UnityEngine;
using System;
using System.Collections.Generic;

/// <summary>
/// Model layer placeholders.
/// These types exist so widgets compile; replace with server-driven models (Packets.g.cs) later.
/// Field names follow the SQL schema used by the server (snake_case where the table does).
/// </summary>
public class NpcInfo
{
    public int id;
    public string name;
    public string title;
    public string name_ru;
    public string title_ru;
    public int faction_id;
    public string emotion;
    public string factionColor = "#FFFFFF";
    public List<string> dialogues = new List<string>();
    public List<QuestInfo> quests = new List<QuestInfo>();
}

public class CityInfo
{
    public int id;
    public string name_ru;
    public string description_ru;
    public int faction_id;
    public List<NpcInfo> npcs = new List<NpcInfo>();
}

public class QuestInfo
{
    public int id;
    public string title_ru;
    public string description_ru;
    public int level_min;
    public int level_max;
    public int reward_value;
    public string reward_type;
    public int faction_id;
    public bool completed;
}

public class CampaignDef
{
    public int id;
    public string name_ru;
    public string name_en;
    public string description_ru;
    public int faction_id;
    public int level_start;
    public int level_end;
    public int difficulty;
    public int estimated_duration_minutes;
    public int required_power;
    public string reward_summary;
    public bool unlocked;
}

public class CampaignData
{
    public int id;
    public string name_ru;
    public string description_ru;
    public int faction_id;
    public int level_start;
    public int level_end;
    public int difficulty;
    public int progress;
    public bool available;

    public CampaignDef ToDef()
    {
        return new CampaignDef
        {
            id = id,
            name_ru = name_ru,
            description_ru = description_ru,
            faction_id = faction_id,
            level_start = level_start,
            level_end = level_end,
            difficulty = difficulty,
            unlocked = available
        };
    }
}

public class AchievementDef
{
    public int id;
    public string name_ru;
    public string description_ru;
    public int condition_value;
    public int reward_points;
}

public class AchievementProgress
{
    public int achievementId;
    public int progress;
    public bool completed;
}

public class AchievementData
{
    public int id;
    public AchievementDef def;
    public AchievementProgress progress;
    public bool claimed;
}

public class ShopItemData
{
    public uint id;
    public string name_ru;
    public int cost_achievement_points;
    public int currency;
    public bool is_limited;
    public int daily_limit;
}

public class ContaminationZone
{
    public int id;
    public string name_ru;
    public float contamination_level;
}

public class PlayerContaminationState
{
    public float infection_level;
    public string debuff_type;
    public float debuff_value;
}

public class PlayerProfile
{
    public ulong guid;
    public string name = "Игрок";
    public int level = 1;
    public int class_id;
    public int race_id;
    public int faction_id;
    public int title_id;
    public long gold;
    public long spirit;
    public long experience;
    public long next_level_experience = 1000;
    public int strength = 10;
    public int agility = 10;
    public int intelligence = 10;
    public int stamina = 10;
    public int spiritStat = 10;
    public int hairStyle;
    public int hairColor;
    public int skinColor;
}
