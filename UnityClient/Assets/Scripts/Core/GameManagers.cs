using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Game state placeholders.
/// Minimal managers so the UI layer compiles and runs; real behaviour arrives with the network layer.
/// </summary>
public class GameManager : MonoBehaviour
{
    public static GameManager Instance { get; private set; }

    public ulong PlayerGuid { get; set; }

    public ChatManager ChatManager { get; private set; }
    public CampaignManager CampaignManager { get; private set; }
    public AchievementManager AchievementManager { get; private set; }
    public ContaminationManager ContaminationManager { get; private set; }
    public HousingManager HousingManager { get; private set; }

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }

        Instance = this;

        ChatManager = new ChatManager();
        CampaignManager = new CampaignManager();
        AchievementManager = new AchievementManager();
        ContaminationManager = new ContaminationManager();
        HousingManager = new HousingManager();
    }
}

public class ChatMessage
{
    public ulong playerGuid;
    public string senderName;
    public string text;
    public string channel;
    public bool isWhisper;
}

public class ChatManager
{
    private const int MaxHistory = 200;

    private readonly List<ChatMessage> _history = new List<ChatMessage>();

    public void SendMessage(ulong playerGuid, string text, string channel, ulong targetGuid)
    {
        if (string.IsNullOrWhiteSpace(text))
            return;

        AddMessage(new ChatMessage
        {
            playerGuid = playerGuid,
            senderName = "Игрок",
            text = text,
            channel = channel,
            isWhisper = channel == "whisper"
        });
    }

    public void AddMessage(ChatMessage message)
    {
        if (message == null)
            return;

        _history.Add(message);
        if (_history.Count > MaxHistory)
            _history.RemoveRange(0, _history.Count - MaxHistory);
    }

    public List<ChatMessage> GetMessageHistory(ulong channelId, int limit)
    {
        int take = Mathf.Min(limit, _history.Count);
        return _history.GetRange(_history.Count - take, take);
    }
}

public class CampaignManager
{
    public List<CampaignData> GetActiveCampaigns()
    {
        return new List<CampaignData>();
    }
}

public class AchievementManager
{
    public List<AchievementData> GetAllAchievements()
    {
        return new List<AchievementData>();
    }

    public Dictionary<int, AchievementProgress> GetPlayerProgress(ulong playerGuid)
    {
        return new Dictionary<int, AchievementProgress>();
    }

    public List<ShopItemData> GetShopItems()
    {
        return new List<ShopItemData>();
    }

    public int GetPointsEarned(ulong playerGuid)
    {
        return 0;
    }

    public int GetPointsNeededForLevel(ulong playerGuid)
    {
        return 100;
    }

    public bool BuyFromShop(ulong playerGuid, uint itemId)
    {
        return false;
    }
}

public class ContaminationManager
{
    public List<ContaminationZone> GetActiveZones()
    {
        return new List<ContaminationZone>();
    }

    public float GetPlayerContamination(ulong playerGuid)
    {
        return 0f;
    }

    public int GetCureCost()
    {
        return 0;
    }

    public bool TryCurePlayer(ulong playerGuid, int amount)
    {
        return false;
    }
}

public class HousingManager
{
    private readonly Dictionary<ulong, PlayerProfile> _profiles = new Dictionary<ulong, PlayerProfile>();

    public bool GetPlayerProfile(ulong guid, out PlayerProfile profile)
    {
        return _profiles.TryGetValue(guid, out profile);
    }

    public PlayerProfile CreatePlayerProfile(ulong guid)
    {
        var profile = new PlayerProfile { guid = guid, name = "Игрок" };
        _profiles[guid] = profile;
        return profile;
    }

    public void UpdatePlayerProfile(ulong guid, PlayerProfile profile)
    {
        if (profile != null)
            _profiles[guid] = profile;
    }
}
