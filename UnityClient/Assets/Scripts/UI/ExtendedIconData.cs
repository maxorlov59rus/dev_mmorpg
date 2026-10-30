using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class ExtendedIconData
    {
        public int Id;
        public string Name;
        public string FilePath;
        public string Category;
        public string Subcategory;
        public int SizePx;
        public int Quality;
        public string DescriptionRu;
        public string DescriptionEn;
    }

    [Unity.Scripting.LifecycleManagement.NoAutoStaticsCleanup]
    public static class ExtendedIconHelper
    {
        public static readonly List<ExtendedIconData> Icons = new List<ExtendedIconData>();

        public static readonly string[] QualityColors = { "#FFFFFF", "#1EFF00", "#0070DD", "#A335EE", "#FF8000" };
        public static readonly string[] CategoryNamesRu = { "", "Навык", "Пассивный", "Оружие", "Броня", "Расходник", "Предмет", "Разное" };
        public static readonly string[] CategoryNamesEn = { "", "Skill", "Passive", "Weapon", "Armor", "Consumable", "Item", "Misc" };

        public static ExtendedIconData GetIcon(int id)
        {
            return Icons.FirstOrDefault(i => i.Id == id);
        }

        public static ExtendedIconData GetIconByName(string name)
        {
            return Icons.FirstOrDefault(i => i.Name == name);
        }

        public static List<ExtendedIconData> GetIconsByCategory(string category)
        {
            return Icons.Where(i => i.Category == category).ToList();
        }
    }
}
