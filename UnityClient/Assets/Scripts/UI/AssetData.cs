using UnityEngine;
using System;
using System.Collections.Generic;

namespace DM.Client
{
    public class MusicTrackData
    {
        public int Id;
        public string NameRu;
        public string NameEn;
        public string FilePath;
        public string Category;
        public int DurationSec;
        public bool LoopTrack;
        public float Volume;
    }

    public class ModelData
    {
        public int Id;
        public string NameRu;
        public string NameEn;
        public string ModelPath;
        public string ModelType;
        public int PolyCount;
        public int TextureSize;
    }

    public class IconData
    {
        public int Id;
        public string Name;
        public string FilePath;
        public string Category;
        public int SizePx;
    }

    public class LoreData
    {
        public int Id;
        public string Category;
        public int EntityId;
        public string TitleRu;
        public string TitleEn;
        public string TextRu;
        public string TextEn;
        public string AuthorRu;
        public string AuthorEn;
    }

    [Unity.Scripting.LifecycleManagement.NoAutoStaticsCleanup]
    public static class AssetDataHelper
    {
        public static readonly List<MusicTrackData> MusicTracks = new List<MusicTrackData>
        {
            new MusicTrackData { Id = 1, NameRu = "Тема главного меню", NameEn = "Main Menu Theme", FilePath = "Audio/Music/main_menu_theme", Category = "ui", DurationSec = 180, LoopTrack = true, Volume = 0.8f },
            new MusicTrackData { Id = 2, NameRu = "Амбиент города", NameEn = "City Ambient", FilePath = "Audio/Music/city_ambient", Category = "ambient", DurationSec = 240, LoopTrack = true, Volume = 0.6f },
            new MusicTrackData { Id = 3, NameRu = "Боевая тема", NameEn = "Combat Theme", FilePath = "Audio/Music/combat_theme", Category = "combat", DurationSec = 90, LoopTrack = true, Volume = 0.9f },
            new MusicTrackData { Id = 4, NameRu = "Босс: Цитадель Тьмы", NameEn = "Boss: Citadel", FilePath = "Audio/Music/boss_citadel", Category = "boss", DurationSec = 120, LoopTrack = true, Volume = 1.0f },
            new MusicTrackData { Id = 5, NameRu = "Босс: Логово Дракона", NameEn = "Boss: Dragon Lair", FilePath = "Audio/Music/boss_dragon", Category = "boss", DurationSec = 150, LoopTrack = true, Volume = 1.0f },
            new MusicTrackData { Id = 6, NameRu = "Данж: Подземелье Забытых", NameEn = "Dungeon: Forgotten", FilePath = "Audio/Music/dungeon_forgotten", Category = "dungeon", DurationSec = 200, LoopTrack = true, Volume = 0.7f },
            new MusicTrackData { Id = 12, NameRu = "PvP: Битва Духов", NameEn = "PvP: Spirit Battle", FilePath = "Audio/Music/pvp_spirit_battle", Category = "event", DurationSec = 120, LoopTrack = true, Volume = 0.9f },
            new MusicTrackData { Id = 15, NameRu = "Победа", NameEn = "Victory", FilePath = "Audio/Music/victory_fanfare", Category = "ui", DurationSec = 15, LoopTrack = false, Volume = 1.0f },
            new MusicTrackData { Id = 16, NameRu = "Поражение", NameEn = "Defeat", FilePath = "Audio/Music/defeat", Category = "ui", DurationSec = 10, LoopTrack = false, Volume = 1.0f },
        };

        public static readonly List<LoreData> LoreEntries = new List<LoreData>
        {
            new LoreData { Id = 1, Category = "world", EntityId = 0, TitleRu = "Сотворение мира", TitleEn = "Creation of the World", TextRu = "В начале был Хаос. Из Хаоса родились три силы: Свет, Тьма и Пустота.", TextEn = "In the beginning there was Chaos. From Chaos three forces were born: Light, Darkness, and Void.", AuthorRu = "Неизвестный", AuthorEn = "Unknown" },
            new LoreData { Id = 2, Category = "faction", EntityId = 1, TitleRu = "Ковенант Рассвета", TitleEn = "Covenant of Dawn", TextRu = "Фракция света и порядка. Основана первыми людьми.", TextEn = "Faction of light and order. Founded by first humans.", AuthorRu = "Летописец Веларион", AuthorEn = "Chronicler Velarion" },
            new LoreData { Id = 5, Category = "class", EntityId = 1, TitleRu = "Воин", TitleEn = "Warrior", TextRu = "Мастера ближнего боя. Сила и выносливость.", TextEn = "Masters of melee. Strength and stamina.", AuthorRu = "Тренер Веларион", AuthorEn = "Trainer Velarion" },
        };
    }
}
