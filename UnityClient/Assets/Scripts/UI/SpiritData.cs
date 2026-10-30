using UnityEngine;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public enum SpiritQuality
    {
        Common = 1,
        Rare = 2,
        Epic = 3,
        Legendary = 4
    }

    public enum SpiritClass
    {
        Guardian = 1,
        Healer = 2,
        Destroyer = 3,
        Support = 4
    }

    public class SpiritSkillData
    {
        public int SkillId;
        public int ClassId;
        public string NameRu;
        public string NameEn;
        public string DamageType;
        public int BaseDamage;
        public int CooldownMs;
        public int ManaCost;
    }

    public class SpiritData
    {
        public ulong SpiritId;
        public uint PlayerGuid;
        public SpiritQuality Quality;
        public SpiritClass SpiritClass;
        public int Level;
        public ulong Experience;
        public List<int> Skills = new List<int>();
        public Dictionary<int, int> SkillLevels = new Dictionary<int, int>();
        public bool IsActive;
        public int RollsUsedToday;
        public uint LastRollDate;
    }

    [Unity.Scripting.LifecycleManagement.NoAutoStaticsCleanup]
    public static class SpiritDataHelper
    {
        public static readonly string[] QualityNamesRu = { "", "Обычный", "Редкий", "Эпический", "Легендарный" };
        public static readonly string[] QualityNamesEn = { "", "Common", "Rare", "Epic", "Legendary" };
        public static readonly string[] QualityColors = { "", "#FFFFFF", "#00FF00", "#9933FF", "#FF8000" };
        public static readonly string[] ClassNamesRu = { "", "Страж", "Целитель", "Разрушитель", "Поддержка" };
        public static readonly string[] ClassNamesEn = { "", "Guardian", "Healer", "Destroyer", "Support" };
        public static readonly string[] ClassRoles = { "", "tank", "healer", "dps", "support" };

        public static readonly List<SpiritSkillData> Skills = new List<SpiritSkillData>
        {
            new SpiritSkillData { SkillId = 1, ClassId = 1, NameRu = "Удар щитом", NameEn = "Shield Bash", DamageType = "physical", BaseDamage = 50, CooldownMs = 8000, ManaCost = 20 },
            new SpiritSkillData { SkillId = 2, ClassId = 1, NameRu = "Провокация", NameEn = "Taunt", DamageType = "physical", BaseDamage = 0, CooldownMs = 10000, ManaCost = 15 },
            new SpiritSkillData { SkillId = 3, ClassId = 1, NameRu = "Каменная кожа", NameEn = "Stone Skin", DamageType = "physical", BaseDamage = 0, CooldownMs = 15000, ManaCost = 30 },
            new SpiritSkillData { SkillId = 4, ClassId = 2, NameRu = "Исцеление", NameEn = "Heal", DamageType = "holy", BaseDamage = 100, CooldownMs = 6000, ManaCost = 25 },
            new SpiritSkillData { SkillId = 5, ClassId = 2, NameRu = "Благословение", NameEn = "Blessing", DamageType = "holy", BaseDamage = 0, CooldownMs = 12000, ManaCost = 20 },
            new SpiritSkillData { SkillId = 6, ClassId = 2, NameRu = "Очищение", NameEn = "Purify", DamageType = "holy", BaseDamage = 0, CooldownMs = 10000, ManaCost = 15 },
            new SpiritSkillData { SkillId = 7, ClassId = 3, NameRu = "Огненный шар", NameEn = "Fireball", DamageType = "fire", BaseDamage = 80, CooldownMs = 5000, ManaCost = 20 },
            new SpiritSkillData { SkillId = 8, ClassId = 3, NameRu = "Ледяная стрела", NameEn = "Frost Bolt", DamageType = "frost", BaseDamage = 60, CooldownMs = 6000, ManaCost = 15 },
            new SpiritSkillData { SkillId = 9, ClassId = 3, NameRu = "Цепная молния", NameEn = "Chain Lightning", DamageType = "nature", BaseDamage = 40, CooldownMs = 8000, ManaCost = 30 },
            new SpiritSkillData { SkillId = 10, ClassId = 4, NameRu = "Боевой клич", NameEn = "Battle Cry", DamageType = "physical", BaseDamage = 0, CooldownMs = 15000, ManaCost = 25 },
            new SpiritSkillData { SkillId = 11, ClassId = 4, NameRu = "Замедление", NameEn = "Slow", DamageType = "shadow", BaseDamage = 0, CooldownMs = 8000, ManaCost = 20 },
            new SpiritSkillData { SkillId = 12, ClassId = 4, NameRu = "Щит группы", NameEn = "Party Shield", DamageType = "holy", BaseDamage = 0, CooldownMs = 20000, ManaCost = 40 },
        };

        public static int GetSkillSlotCount(SpiritQuality quality)
        {
            switch (quality)
            {
                case SpiritQuality.Common: return 1;
                case SpiritQuality.Rare: return 2;
                case SpiritQuality.Epic: return 3;
                case SpiritQuality.Legendary: return 4;
            }
            return 1;
        }

        public static int GetExpForLevel(int level)
        {
            return level * 100;
        }
    }
}
