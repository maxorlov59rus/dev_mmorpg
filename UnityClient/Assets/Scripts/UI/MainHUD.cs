using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Main HUD - основной интерфейс игры (заголовок экрана)
/// </summary>
public class MainHUD : MonoBehaviour
{
    [Header("UI References")]
    public Text levelText;
    public Text experienceText;
    public Text goldText;
    public Text spiritText;
    public Text factionText;
    public Text timerText;
    
    [Header("Status Bars")]
    public GameObject healthBar;
    public GameObject manaBar;
    public GameObject staminaBar;
    
    [Header("Mini-Map")]
    public RectTransform miniMap;
    public Transform playerMarker;
    
    private uint64 playerGuid;
    private PlayerProfile currentProfile;
    
    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        InitializeHUD();
    }
    
    void InitializeHUD()
    {
        // Find player profile
        var housingMgr = GameManager.Instance?.HousingManager;
        if (housingMgr != null)
        {
            housingMgr.GetPlayerProfile(playerGuid, out currentProfile);
        }
        UpdateHUD();
    }
    
    void UpdateHUD()
    {
        if (currentProfile != null)
        {
            // Level and experience
            if (levelText != null)
                levelText.text = $"Уровень: {currentProfile.level}";
            
            if (experienceText != null)
                experienceText.text = $"Опыт: {currentProfile.experience}/{currentProfile.next_level_experience}";
            
            // Gold and Spirit
            if (goldText != null)
                goldText.text = $"Золото: {currentProfile.gold}";
            
            if (spiritText != null)
                spiritText.text = $"Дух: {currentProfile.spirit}";
            
            // Faction
            if (factionText != null)
                factionText.text = $"Фракция: {currentProfile.faction_id}";
        }
        
        // Update status bars
        UpdateStatusBars();
    }
    
    void UpdateStatusBars()
    {
        // Health bar
        if (healthBar != null)
        {
            // Would calculate based on current/max HP
            healthBar.transform.localScale = new Vector3(0.5f, 1f, 1f); // Placeholder
        }
        
        // Mana bar
        if (manaBar != null)
        {
            manaBar.transform.localScale = new Vector3(0.5f, 1f, 1f); // Placeholder
        }
        
        // Stamina bar
        if (staminaBar != null)
        {
            staminaBar.transform.localScale = new Vector3(0.5f, 1f, 1f); // Placeholder
        }
    }
    
    // Call this to update HUD elements dynamically
    public void Refresh()
    {
        if (GameManager.Instance != null)
        {
            var housingMgr = GameManager.Instance.HousingManager;
            housingMgr.GetPlayerProfile(playerGuid, out currentProfile);
            UpdateHUD();
        }
    }
}