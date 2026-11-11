using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Слой данных клиента: персонажи, инвентарь, квесты, кампании.
/// Работает в локальном режиме; серверные менеджеры подключаются позже через те же интерфейсы.
/// </summary>
public class GameData
{
    public static GameData Instance { get; private set; } = new GameData();

    public PlayerProfile Profile { get; private set; }
    public List<CharacterSlot> Characters { get; private set; }
    public List<InventoryItem> Inventory { get; private set; }
    public List<QuestInfo> Quests { get; private set; }
    public List<CampaignData> Campaigns { get; private set; }

    public event System.Action OnInventoryChanged;
    public event System.Action OnQuestChanged;
    public event System.Action OnProfileChanged;

    public GameData()
    {
        Characters = new List<CharacterSlot>
        {
            new CharacterSlot { name = "Странник", level = 70, cls = "Жрец", faction = "Орден Света" },
            new CharacterSlot { name = "Таллон", level = 48, cls = "Маг", faction = "Тёмный Орден" },
            new CharacterSlot { name = "Вилл", level = 12, cls = "Плут", faction = "Нейтральные" },
        };

        Profile = new PlayerProfile
        {
            guid = 1,
            name = "Странник",
            level = 70,
            class_id = 3,
            race_id = 1,
            faction_id = 1,
            gold = 1250,
            spirit = 40,
            experience = 4200,
            next_level_experience = 9000,
        };

        Inventory = new List<InventoryItem>
        {
            new InventoryItem("Клинок Света", 1, "Оружие", 1, "Урон 120–140"),
            new InventoryItem("Щит Рассвета", 1, "Щит", 1, "Броня +45"),
            new InventoryItem("Зелье лечения", 5, "Расходник", 1, "Восстанавливает 300 HP"),
            new InventoryItem("Свиток возврата", 2, "Расходник", 1, "Возврат в Ательгард"),
            new InventoryItem("Кольцо силы", 1, "Аксессуар", 1, "+12 к силе"),
            new InventoryItem("Руда мифрила", 12, "Ресурс", 12, "Для кузнечного дела"),
            new InventoryItem("Кожа волка", 8, "Ресурс", 8, "Для кожевничества"),
            new InventoryItem("Ключ подземелья", 1, "Ключ", 1, "Открывает Сумеречный склеп"),
            new InventoryItem("Осколок духа", 3, "Дух", 3, "Усиливает призыва духа"),
            new InventoryItem("Карта Ательгарда", 1, "Документ", 1, "Открывает локации города"),
            new InventoryItem("Эликсир стойкости", 4, "Расходник", 4, "+50 выносливости на 10 мин"),
            new InventoryItem("Амулет духа", 1, "Аксессуар", 1, "+8 к духовности"),
        };

        Quests = new List<QuestInfo>
        {
            new QuestInfo { id = 1001, title_ru = "Очищение окраин", description_ru = "Очистите заражённые земли Ательгарда от скверны.", level_min = 70, level_max = 75, reward_value = 500, reward_type = "золота", faction_id = 1 },
            new QuestInfo { id = 1002, title_ru = "Свет над руинами", description_ru = "Восстановите солнечный храм на востоке.", level_min = 75, level_max = 80, reward_value = 800, reward_type = "золота", faction_id = 1 },
            new QuestInfo { id = 1003, title_ru = "Голос Умбры", description_ru = "Выслушайте Великого Мага Таллона.", level_min = 70, level_max = 75, reward_value = 3, reward_type = "духа", faction_id = 2 },
        };

        Campaigns = new List<CampaignData>
        {
            new CampaignData { id = 1, name_ru = "Очищение", faction_id = 1, level_start = 70, level_end = 75, difficulty = 1, available = true },
            new CampaignData { id = 2, name_ru = "Восхождение Света", faction_id = 1, level_start = 75, level_end = 80, difficulty = 2, available = true },
            new CampaignData { id = 5, name_ru = "Восхождение Тьмы", faction_id = 2, level_start = 70, level_end = 75, difficulty = 2, available = true },
            new CampaignData { id = 7, name_ru = "Путь Баланса", faction_id = 3, level_start = 70, level_end = 75, difficulty = 2, available = true },
            new CampaignData { id = 10, name_ru = "Финальное пророчество", faction_id = 3, level_start = 90, level_end = 99, difficulty = 4, available = false },
        };
    }

    // ---------------------------------------------------------- characters

    public CharacterSlot CreateCharacter(string name, string race, string cls, string faction)
    {
        var slot = new CharacterSlot
        {
            name = string.IsNullOrWhiteSpace(name) ? "Странник" : name,
            level = 1,
            cls = cls,
            faction = faction,
            race = race,
        };
        Characters.Add(slot);
        return slot;
    }

    public void SelectCharacter(CharacterSlot slot)
    {
        if (slot == null) return;

        Profile.name = slot.name;
        Profile.level = slot.level;
        OnProfileChanged?.Invoke();
    }

    // ---------------------------------------------------------- inventory

    /// <summary>Убирает предмет из сумки и возвращает false, если столько нет.</summary>
    public bool ConsumeItem(string itemName, int amount = 1)
    {
        var item = Inventory.Find(i => i.name == itemName);
        if (item == null || item.count < amount)
            return false;

        item.count -= amount;
        if (item.count <= 0)
            Inventory.Remove(item);

        OnInventoryChanged?.Invoke();
        return true;
    }

    public bool AddGold(long amount)
    {
        Profile.gold += amount;
        OnProfileChanged?.Invoke();
        return true;
    }

    public bool SpendGold(long amount)
    {
        if (Profile.gold < amount) return false;
        Profile.gold -= amount;
        OnProfileChanged?.Invoke();
        return true;
    }

    // ---------------------------------------------------------- quests

    public void AcceptQuest(QuestInfo quest)
    {
        if (quest == null || Quests.Contains(quest)) return;

        Quests.Add(quest);
        OnQuestChanged?.Invoke();
    }

    public void CompleteQuest(QuestInfo quest)
    {
        if (quest == null) return;

        Quests.Remove(quest);
        Profile.experience += quest.reward_value * 10;
        OnQuestChanged?.Invoke();
        OnProfileChanged?.Invoke();
    }

    public QuestInfo FindQuest(int id)
    {
        return Quests.Find(q => q.id == id);
    }
}

public class CharacterSlot
{
    public string name;
    public int level;
    public string cls;
    public string race;
    public string faction;
}

public class InventoryItem
{
    public string name;
    public int count;
    public string slot;
    public int maxStack;
    public string description;

    public InventoryItem(string name, int count, string slot, int maxStack, string description = null)
    {
        this.name = name;
        this.count = count;
        this.slot = slot;
        this.maxStack = maxStack;
        this.description = description;
    }
}
