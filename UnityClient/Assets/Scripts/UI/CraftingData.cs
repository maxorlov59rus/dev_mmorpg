using UnityEngine;
using DarkMatter.Network;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public enum ProfessionType
    {
        Mining = 1,
        Herbalism = 2,
        Skinning = 3,
        Blacksmithing = 4,
        Alchemy = 5,
        Tailoring = 6,
        Engineering = 7,
        Jewelcrafting = 8
    }

    public enum ProfessionCategory
    {
        Gathering = 1,
        Crafting = 2
    }

    public class ProfessionData
    {
        public int Id;
        public string NameRu;
        public string NameEn;
        public ProfessionCategory Category;
        public int MinLevel;
        public int MaxSkill;
        public string Icon;
    }

    public class RecipeMaterial
    {
        public int ItemId;
        public int Count;
    }

    public class RecipeData
    {
        public int Id;
        public int ProfessionId;
        public string NameRu;
        public string NameEn;
        public int ResultItemId;
        public int ResultCount;
        public int RequiredSkill;
        public int CraftTimeMs;
        public string DescriptionRu;
        public string DescriptionEn;
        public List<RecipeMaterial> Materials = new List<RecipeMaterial>();
    }

    [Unity.Scripting.LifecycleManagement.NoAutoStaticsCleanup]
    public class CraftingData
    {
        public static readonly List<ProfessionData> Professions = new List<ProfessionData>
        {
            new ProfessionData { Id = 1, NameRu = "Горное дело", NameEn = "Mining", Category = ProfessionCategory.Gathering, MinLevel = 20, MaxSkill = 300, Icon = "icon_mining" },
            new ProfessionData { Id = 2, NameRu = "Травничество", NameEn = "Herbalism", Category = ProfessionCategory.Gathering, MinLevel = 20, MaxSkill = 300, Icon = "icon_herbalism" },
            new ProfessionData { Id = 3, NameRu = "Снятие шкур", NameEn = "Skinning", Category = ProfessionCategory.Gathering, MinLevel = 20, MaxSkill = 300, Icon = "icon_skinning" },
            new ProfessionData { Id = 4, NameRu = "Кузнечное дело", NameEn = "Blacksmithing", Category = ProfessionCategory.Crafting, MinLevel = 20, MaxSkill = 300, Icon = "icon_blacksmithing" },
            new ProfessionData { Id = 5, NameRu = "Алхимия", NameEn = "Alchemy", Category = ProfessionCategory.Crafting, MinLevel = 20, MaxSkill = 300, Icon = "icon_alchemy" },
            new ProfessionData { Id = 6, NameRu = "Портняжное дело", NameEn = "Tailoring", Category = ProfessionCategory.Crafting, MinLevel = 20, MaxSkill = 300, Icon = "icon_tailoring" },
            new ProfessionData { Id = 7, NameRu = "Инженерия", NameEn = "Engineering", Category = ProfessionCategory.Crafting, MinLevel = 20, MaxSkill = 300, Icon = "icon_engineering" },
            new ProfessionData { Id = 8, NameRu = "Ювелирное дело", NameEn = "Jewelcrafting", Category = ProfessionCategory.Crafting, MinLevel = 20, MaxSkill = 300, Icon = "icon_jewelcrafting" },
        };
        public static readonly List<RecipeData> Recipes = new List<RecipeData>
        {
            // Blacksmithing
            new RecipeData { Id = 4001, ProfessionId = 4, NameRu = "Железный меч", NameEn = "Iron Sword", ResultItemId = 14001, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 3000, DescriptionRu = "Базовый меч для воинов.", DescriptionEn = "Basic sword for warriors." },
            new RecipeData { Id = 4002, ProfessionId = 4, NameRu = "Железный щит", NameEn = "Iron Shield", ResultItemId = 14002, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 3000, DescriptionRu = "Базовый щит для воинов.", DescriptionEn = "Basic shield for warriors." },
            new RecipeData { Id = 4003, ProfessionId = 4, NameRu = "Стальной меч", NameEn = "Steel Sword", ResultItemId = 14003, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 4000, DescriptionRu = "Улучшенный меч.", DescriptionEn = "Improved sword." },
            new RecipeData { Id = 4004, ProfessionId = 4, NameRu = "Стальной щит", NameEn = "Steel Shield", ResultItemId = 14004, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 4000, DescriptionRu = "Улучшенный щит.", DescriptionEn = "Improved shield." },
            new RecipeData { Id = 4005, ProfessionId = 4, NameRu = "Мифриловый меч", NameEn = "Mithril Sword", ResultItemId = 14005, ResultCount = 1, RequiredSkill = 100, CraftTimeMs = 5000, DescriptionRu = "Мощный меч из мифрила.", DescriptionEn = "Powerful mithril sword." },
            new RecipeData { Id = 4006, ProfessionId = 4, NameRu = "Мифриловый щит", NameEn = "Mithril Shield", ResultItemId = 14006, ResultCount = 1, RequiredSkill = 100, CraftTimeMs = 5000, DescriptionRu = "Мощный щит из мифрила.", DescriptionEn = "Powerful mithril shield." },
            new RecipeData { Id = 4007, ProfessionId = 4, NameRu = "Титановый меч", NameEn = "Titanium Sword", ResultItemId = 14007, ResultCount = 1, RequiredSkill = 200, CraftTimeMs = 6000, DescriptionRu = "Легендарный меч из титана.", DescriptionEn = "Legendary titanium sword." },
            new RecipeData { Id = 4008, ProfessionId = 4, NameRu = "Титановый щит", NameEn = "Titanium Shield", ResultItemId = 14008, ResultCount = 1, RequiredSkill = 200, CraftTimeMs = 6000, DescriptionRu = "Легендарный щит из титана.", DescriptionEn = "Legendary titanium shield." },

            // Alchemy
            new RecipeData { Id = 5001, ProfessionId = 5, NameRu = "Зелье исцеления (малое)", NameEn = "Small Healing Potion", ResultItemId = 14010, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Восстанавливает 100 HP.", DescriptionEn = "Restores 100 HP." },
            new RecipeData { Id = 5002, ProfessionId = 5, NameRu = "Зелье исцеления (среднее)", NameEn = "Medium Healing Potion", ResultItemId = 14011, ResultCount = 1, RequiredSkill = 25, CraftTimeMs = 2500, DescriptionRu = "Восстанавливает 300 HP.", DescriptionEn = "Restores 300 HP." },
            new RecipeData { Id = 5003, ProfessionId = 5, NameRu = "Зелье исцеления (большое)", NameEn = "Large Healing Potion", ResultItemId = 14012, ResultCount = 1, RequiredSkill = 75, CraftTimeMs = 3000, DescriptionRu = "Восстанавливает 800 HP.", DescriptionEn = "Restores 800 HP." },
            new RecipeData { Id = 5004, ProfessionId = 5, NameRu = "Зелье маны (малое)", NameEn = "Small Mana Potion", ResultItemId = 14013, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Восстанавливает 100 маны.", DescriptionEn = "Restores 100 mana." },
            new RecipeData { Id = 5005, ProfessionId = 5, NameRu = "Зелье маны (среднее)", NameEn = "Medium Mana Potion", ResultItemId = 14014, ResultCount = 1, RequiredSkill = 25, CraftTimeMs = 2500, DescriptionRu = "Восстанавливает 300 маны.", DescriptionEn = "Restores 300 mana." },
            new RecipeData { Id = 5006, ProfessionId = 5, NameRu = "Зелье маны (большое)", NameEn = "Large Mana Potion", ResultItemId = 14015, ResultCount = 1, RequiredSkill = 75, CraftTimeMs = 3000, DescriptionRu = "Восстанавливает 800 маны.", DescriptionEn = "Restores 800 mana." },
            new RecipeData { Id = 5007, ProfessionId = 5, NameRu = "Зелье силы", NameEn = "Strength Potion", ResultItemId = 14016, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "+10 силы на 30 мин.", DescriptionEn = "+10 strength for 30 min." },
            new RecipeData { Id = 5008, ProfessionId = 5, NameRu = "Зелье ловкости", NameEn = "Agility Potion", ResultItemId = 14017, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "+10 ловкости на 30 мин.", DescriptionEn = "+10 agility for 30 min." },
            new RecipeData { Id = 5009, ProfessionId = 5, NameRu = "Зелье интеллекта", NameEn = "Intellect Potion", ResultItemId = 14018, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "+10 интеллекта на 30 мин.", DescriptionEn = "+10 intellect for 30 min." },
            new RecipeData { Id = 5010, ProfessionId = 5, NameRu = "Зелье выносливости", NameEn = "Vitality Potion", ResultItemId = 14019, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "+10 выносливости на 30 мин.", DescriptionEn = "+10 vitality for 30 min." },
            new RecipeData { Id = 5011, ProfessionId = 5, NameRu = "Зелье духа", NameEn = "Spirit Potion", ResultItemId = 14020, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "+10 духа на 30 мин.", DescriptionEn = "+10 spirit for 30 min." },
            new RecipeData { Id = 5012, ProfessionId = 5, NameRu = "Эликсир бессмертия", NameEn = "Elixir of Immortality", ResultItemId = 14021, ResultCount = 1, RequiredSkill = 150, CraftTimeMs = 5000, DescriptionRu = "Восстанавливает 50% HP при смерти.", DescriptionEn = "Restores 50% HP on death." },
            new RecipeData { Id = 5013, ProfessionId = 5, NameRu = "Эликсир поглощения", NameEn = "Elixir of Absorption", ResultItemId = 14022, ResultCount = 1, RequiredSkill = 150, CraftTimeMs = 5000, DescriptionRu = "+20% к сопротивлению тьме на 1 час.", DescriptionEn = "+20% dark resistance for 1 hour." },

            // Tailoring
            new RecipeData { Id = 6001, ProfessionId = 6, NameRu = "Льняная рубаха", NameEn = "Linen Shirt", ResultItemId = 14030, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Базовая тканевая броня.", DescriptionEn = "Basic cloth armor." },
            new RecipeData { Id = 6002, ProfessionId = 6, NameRu = "Шёлковая рубаха", NameEn = "Silk Shirt", ResultItemId = 14031, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 3000, DescriptionRu = "Улучшенная тканевая броня.", DescriptionEn = "Improved cloth armor." },
            new RecipeData { Id = 6003, ProfessionId = 6, NameRu = "Магическая рубаха", NameEn = "Magic Shirt", ResultItemId = 14032, ResultCount = 1, RequiredSkill = 100, CraftTimeMs = 4000, DescriptionRu = "Магическая тканевая броня.", DescriptionEn = "Magical cloth armor." },
            new RecipeData { Id = 6004, ProfessionId = 6, NameRu = "Эпическая рубаха", NameEn = "Epic Shirt", ResultItemId = 14033, ResultCount = 1, RequiredSkill = 200, CraftTimeMs = 5000, DescriptionRu = "Эпическая тканевая броня.", DescriptionEn = "Epic cloth armor." },
            // Engineering
            new RecipeData { Id = 7001, ProfessionId = 7, NameRu = "Дефибриллятор", NameEn = "Defibrillator", ResultItemId = 14040, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 3000, DescriptionRu = "Самовоскрешение инженера.", DescriptionEn = "Engineer self-resurrection." },
            new RecipeData { Id = 7002, ProfessionId = 7, NameRu = "Взрывчатка", NameEn = "Explosive", ResultItemId = 14041, ResultCount = 1, RequiredSkill = 25, CraftTimeMs = 2000, DescriptionRu = "Взрывчатка для боя.", DescriptionEn = "Combat explosive." },
            new RecipeData { Id = 7003, ProfessionId = 7, NameRu = "Гаджет невидимости", NameEn = "Invisibility Gadget", ResultItemId = 14042, ResultCount = 1, RequiredSkill = 50, CraftTimeMs = 4000, DescriptionRu = "Временная невидимость.", DescriptionEn = "Temporary invisibility." },
            new RecipeData { Id = 7004, ProfessionId = 7, NameRu = "Ракетный ранец", NameEn = "Rocket Pack", ResultItemId = 14043, ResultCount = 1, RequiredSkill = 100, CraftTimeMs = 5000, DescriptionRu = "Прыжок на 20 метров.", DescriptionEn = "Jump 20 meters." },
            new RecipeData { Id = 7005, ProfessionId = 7, NameRu = "Турель", NameEn = "Turret", ResultItemId = 14044, ResultCount = 1, RequiredSkill = 150, CraftTimeMs = 6000, DescriptionRu = "Автоматическая турель.", DescriptionEn = "Automatic turret." },
            // Jewelcrafting
            new RecipeData { Id = 8001, ProfessionId = 8, NameRu = "Кольцо силы", NameEn = "Ring of Strength", ResultItemId = 14050, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Кольцо +5 силы.", DescriptionEn = "Ring +5 strength." },
            new RecipeData { Id = 8002, ProfessionId = 8, NameRu = "Кольцо ловкости", NameEn = "Ring of Agility", ResultItemId = 14051, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Кольцо +5 ловкости.", DescriptionEn = "Ring +5 agility." },
            new RecipeData { Id = 8003, ProfessionId = 8, NameRu = "Кольцо интеллекта", NameEn = "Ring of Intellect", ResultItemId = 14052, ResultCount = 1, RequiredSkill = 1, CraftTimeMs = 2000, DescriptionRu = "Кольцо +5 интеллекта.", DescriptionEn = "Ring +5 intellect." },
            new RecipeData { Id = 8004, ProfessionId = 8, NameRu = "Амулет выносливости", NameEn = "Amulet of Vitality", ResultItemId = 14053, ResultCount = 1, RequiredSkill = 25, CraftTimeMs = 3000, DescriptionRu = "Амулет +10 выносливости.", DescriptionEn = "Amulet +10 vitality." },
            new RecipeData { Id = 8005, ProfessionId = 8, NameRu = "Амулет духа", NameEn = "Amulet of Spirit", ResultItemId = 14054, ResultCount = 1, RequiredSkill = 25, CraftTimeMs = 3000, DescriptionRu = "Амулет +10 духа.", DescriptionEn = "Amulet +10 spirit." },
            new RecipeData { Id = 8006, ProfessionId = 8, NameRu = "Кольцо мощи", NameEn = "Ring of Power", ResultItemId = 14055, ResultCount = 1, RequiredSkill = 100, CraftTimeMs = 5000, DescriptionRu = "Кольцо +15 ко всем статам.", DescriptionEn = "Ring +15 to all stats." },
            new RecipeData { Id = 8007, ProfessionId = 8, NameRu = "Амулет бессмертия", NameEn = "Amulet of Immortality", ResultItemId = 14056, ResultCount = 1, RequiredSkill = 200, CraftTimeMs = 6000, DescriptionRu = "Амулет +30 выносливости.", DescriptionEn = "Amulet +30 vitality." },
        };
    }
}
