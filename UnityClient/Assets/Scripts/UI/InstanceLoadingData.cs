using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public enum LoadingAnimationType
    {
        Fade = 1,
        Slide = 2,
        Zoom = 3,
        Particle = 4
    }

    public enum InstanceType
    {
        Dungeon = 1,
        Raid = 2,
        Instance = 3
    }

    public class LoadingScreenData
    {
        public int Id;
        public InstanceType InstanceType;
        public int MapId;
        public string NameRu;
        public string NameEn;
        public string BackgroundImage;
        public LoadingAnimationType AnimationType;
        public int AnimationDurationMs;
        public string TipRu;
        public string TipEn;
        public string MusicTrack;
        public int MinLevel;
        public bool IsActive;
    }

    [Unity.Scripting.LifecycleManagement.NoAutoStaticsCleanup]
    public static class LoadingScreenHelper
    {
        public static readonly List<LoadingScreenData> Screens = new List<LoadingScreenData>
        {
            new LoadingScreenData { Id = 1, InstanceType = InstanceType.Dungeon, MapId = 2001, NameRu = "Подземелье Забытых", NameEn = "Dungeon of the Forgotten", BackgroundImage = "bg_dungeon_forgotten", AnimationType = LoadingAnimationType.Fade, AnimationDurationMs = 3000, TipRu = "Используйте факелы для освещения тёмных коридоров.", TipEn = "Use torches to light dark corridors.", MusicTrack = "music_dungeon_ambient", MinLevel = 10, IsActive = true },
            new LoadingScreenData { Id = 2, InstanceType = InstanceType.Dungeon, MapId = 2002, NameRu = "Пещеры Теней", NameEn = "Caves of Shadows", BackgroundImage = "bg_dungeon_shadows", AnimationType = LoadingAnimationType.Slide, AnimationDurationMs = 3500, TipRu = "Остерегайтесь ловушек на полу.", TipEn = "Beware of floor traps.", MusicTrack = "music_dungeon_dark", MinLevel = 15, IsActive = true },
            new LoadingScreenData { Id = 3, InstanceType = InstanceType.Dungeon, MapId = 2003, NameRu = "Руины Древних", NameEn = "Ruins of the Ancients", BackgroundImage = "bg_dungeon_ruins", AnimationType = LoadingAnimationType.Zoom, AnimationDurationMs = 4000, TipRu = "Ищите скрытые проходы за стенами.", TipEn = "Look for hidden passages behind walls.", MusicTrack = "music_dungeon_epic", MinLevel = 20, IsActive = true },
            new LoadingScreenData { Id = 4, InstanceType = InstanceType.Raid, MapId = 3001, NameRu = "Рейд: Цитадель Тьмы", NameEn = "Raid: Citadel of Darkness", BackgroundImage = "bg_raid_citadel", AnimationType = LoadingAnimationType.Particle, AnimationDurationMs = 5000, TipRu = "Координация группы критически важна.", TipEn = "Group coordination is critical.", MusicTrack = "music_raid_epic", MinLevel = 30, IsActive = true },
            new LoadingScreenData { Id = 5, InstanceType = InstanceType.Raid, MapId = 3002, NameRu = "Рейд: Логово Дракона", NameEn = "Raid: Dragon Lair", BackgroundImage = "bg_raid_dragon", AnimationType = LoadingAnimationType.Fade, AnimationDurationMs = 4500, TipRu = "Избегайте огненного дыхания дракона.", TipEn = "Avoid the dragon breath.", MusicTrack = "music_raid_dragon", MinLevel = 40, IsActive = true },
            new LoadingScreenData { Id = 6, InstanceType = InstanceType.Instance, MapId = 4001, NameRu = "Инстанс: Разлом", NameEn = "Instance: Rift", BackgroundImage = "bg_instance_rift", AnimationType = LoadingAnimationType.Slide, AnimationDurationMs = 3000, TipRu = "Разлом искажает пространство и время.", TipEn = "The rift distorts space and time.", MusicTrack = "music_instance_mystic", MinLevel = 25, IsActive = true },
            new LoadingScreenData { Id = 7, InstanceType = InstanceType.Instance, MapId = 4002, NameRu = "Инстанс: Астральный План", NameEn = "Instance: Astral Plane", BackgroundImage = "bg_instance_astral", AnimationType = LoadingAnimationType.Zoom, AnimationDurationMs = 3500, TipRu = "Астральные существа невидимы для обычных атак.", TipEn = "Astral beings are invisible to normal attacks.", MusicTrack = "music_instance_cosmic", MinLevel = 35, IsActive = true },
        };

        public static LoadingScreenData GetScreenForMap(int mapId)
        {
            return Screens.FirstOrDefault(s => s.MapId == mapId);
        }
    }
}
