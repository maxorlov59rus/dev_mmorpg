using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// Achievement Widget - displays player achievements and shop
/// </summary>
public class AchievementWidget : MonoBehaviour
{
    [Header("UI References")]
    public Text achievementsTitle;
    public Text progressTitle;
    public Transform achievementsContent;
    public GameObject achievementTemplate;
    public Text shopTitle;
    public Transform shopContent;
    public GameObject shopTemplate;
    public Text pointsDisplay;
    public Text pointsEarnedDisplay;
    
    [Header("Buttons")]
    public Button closeButton;
    public Button claimAllButton;

    private uint64 playerGuid;
    private AchievementManager achievementManager;
    private List<AchievementData> displayedAchievements = new List<AchievementData>();
    private Dictionary<uint32, ShopItemData> shopItems = new Dictionary<uint32, ShopItemData>();

    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        achievementManager = GameManager.Instance?.AchievementManager;
        
        if (achievementManager == null)
        {
            Debug.LogError("AchievementManager not found!");
            enabled = false;
            return;
        }

        InitializeUI();
        LoadAchievements();
        LoadShopItems();
    }

    void InitializeUI()
    {
        // Setup close button
        closeButton.onClick.AddListener(CloseWindow);

        // Setup templates - hide by default
        if (achievementTemplate != null)
            achievementTemplate.SetActive(false);
        
        if (shopTemplate != null)
            shopTemplate.SetActive(false);

        // Update points display
        UpdatePointsDisplay();
    }

    void LoadAchievements()
    {
        if (achievementManager == null) return;

        var allAchievements = achievementManager.GetAllAchievements();
        var playerProgress = achievementManager.GetPlayerProgress(playerGuid);

        // Show title
        if (achievementsTitle != null)
            achievementsTitle.text = "Достижения";

        // Clear existing content
        if (achievementsContent != null)
        {
            foreach (Transform child in achievementsContent)
                Destroy(child.gameObject);
            displayedAchievements.Clear();
        }

        // Create achievement entries
        foreach (var def in allAchievements)
        {
            var progress = GetPlayerProgressForAchievement(playerProgress, def.id);
            var go = Instantiate(achievementTemplate, achievementsContent);
            go.SetActive(true);
            
            var data = go.AddComponent<AchievementEntry>();
            data.Initialize(def, progress);
            displayedAchievements.Add(data);
        }

        // Update points
        UpdatePointsDisplay();
    }

    AchievementProgress GetPlayerProgressForAchievement(List<AchievementProgress> progressList, uint32 achievementId)
    {
        foreach (var p in progressList)
        {
            if (p.achievementId == achievementId)
                return p;
        }
        return new AchievementProgress { progress = 0, completed = false };
    }

    void LoadShopItems()
    {
        if (shopTitle != null)
            shopTitle.text = "Магазин достижений";

        // Clear existing content
        if (shopContent != null)
        {
            foreach (Transform child in shopContent)
                Destroy(child.gameObject);
            shopItems.Clear();
        }

        // Load shop from database
        var dbShop = achievementManager.GetShopItems();
        
        foreach (var shopItem in dbShop)
        {
            var go = Instantiate(shopTemplate, shopContent);
            go.SetActive(true);
            
            var data = go.AddComponent<ShopItem>();
            data.Initialize(shopItem);
            shopItems[shopItem.id] = data;
        }
    }

    void UpdatePointsDisplay()
    {
        if (pointsDisplay != null && pointsEarnedDisplay != null)
        {
            var earned = achievementManager.GetPointsEarned(playerGuid);
            var total = achievementManager.GetPointsNeededForLevel(playerGuid);
            
            pointsDisplay.text = $"Очки: {earned}/{total}";
            pointsEarnedDisplay.text = $"Прогресс: {earned} очков Achievement";
        }
    }

    public void OpenWindow(uint64 guid)
    {
        playerGuid = guid;
        gameObject.SetActive(true);
        LoadAchievements();
        LoadShopItems();
    }

    public void CloseWindow()
    {
        gameObject.SetActive(false);
    }

    public void OnAchievementClaimed(uint32 achievementId)
    {
        // Refresh the UI
        LoadAchievements();
    }
}