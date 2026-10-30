using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Shop item entry - achievements shop
/// </summary>
public class ShopItem : MonoBehaviour
{
    [Header("UI References")]
    public Text itemName;
    public Text itemCost;
    public Button buyButton;
    public GameObject limitedBadge;
    public Text limitText;

    private ShopItemData itemData;
    private AchievementManager achievementManager;
    private uint64 playerGuid;

    public void Initialize(ShopItemData data)
    {
        itemData = data;
        
        if (itemName != null)
            itemName.text = data.name_ru;

        if (itemCost != null)
            itemCost.text = $"{data.cost_achievement_points} очков";

        if (limitedBadge != null)
            limitedBadge.SetActive(data.is_limited);

        if (limitText != null && data.is_limited)
            limitText.text = $"Лимит: {data.daily_limit} в день";

        // Setup buy button
        if (buyButton != null)
        {
            buyButton.onClick.RemoveAllListeners();
            buyButton.onClick.AddListener(OnBuyClicked);
        }
    }

    private void OnBuyClicked()
    {
        if (achievementManager == null)
            achievementManager = GameManager.Instance?.AchievementManager;

        if (achievementManager == null)
        {
            Debug.LogError("AchievementManager not found!");
            return;
        }

        // Try to buy from shop
        var result = achievementManager.BuyFromShop(playerGuid, itemData.id);
        
        if (result)
        {
            // Purchase successful - remove or disable the button
            buyButton.interactable = false;
            buyButton.transform.parent.parent.Find("bought-mark")?.SetActive(true);
            
            // Refresh shop items
            var widget = GetComponentInParent<AchievementWidget>();
            if (widget != null)
                widget.LoadShopItems();
        }
        else
        {
            Debug.Log("Not enough achievement points!");
        }
    }
}