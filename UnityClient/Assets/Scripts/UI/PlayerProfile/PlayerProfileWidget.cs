using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

/// <summary>
/// Player Profile Widget - профиль игрока
/// </summary>
public class PlayerProfileWidget : MonoBehaviour
{
    [Header("UI References")]
    public Text playerNameText;
    public Text playerLevelText;
    public Text playerClassText;
    public Text playerRaceText;
    public Text playerFactionText;
    public Text playerGoldText;
    public Text playerSpiritText;
    public Text playerTitleText;
    
    [Header("Stat Bars")]
    public GameObject strengthBar;
    public GameObject agilityBar;
    public GameObject intelligenceBar;
    public GameObject staminaBar;
    public GameObject spiritBar;
    
    [Header("Avatar Visuals")]
    public RawImage avatarImage;
    public Dropdown hairStyleDropdown;
    public Dropdown hairColorDropdown;
    public Dropdown skinColorDropdown;
    
    private uint64 playerGuid;
    private PlayerProfile currentProfile;
    
    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        LoadPlayerProfile();
        SetupDropdowns();
    }
    
    void LoadPlayerProfile()
    {
        var housingMgr = GameManager.Instance?.HousingManager;
        if (housingMgr == null) return;
        
        if (housingMgr.GetPlayerProfile(playerGuid, out currentProfile))
        {
            UpdateProfileUI();
        }
        else
        {
            // Create default profile
            CreateDefaultProfile();
        }
    }
    
    void CreateDefaultProfile()
    {
        housingMgr.CreatePlayerProfile(playerGuid);
        housingMgr.GetPlayerProfile(playerGuid, out currentProfile);
        UpdateProfileUI();
    }
    
    void UpdateProfileUI()
    {
        if (playerNameText != null)
            playerNameText.text = "Игрок"; // Would get from database
        
        if (playerLevelText != null)
            playerLevelText.text = $"Уровень: {currentProfile.level}";
        
        if (playerClassText != null)
        {
            // Would lookup class name from database
            playerClassText.text = "Класс: " + currentProfile.class_id;
        }
        
        if (playerRaceText != null)
        {
            // Would lookup race name from database
            playerRaceText.text = "Раса: " + currentProfile.race_id;
        }
        
        if (playerFactionText != null)
        {
            // Would lookup faction name from database
            playerFactionText.text = "Фракция: " + currentProfile.faction_id;
        }
        
        if (playerGoldText != null)
            playerGoldText.text = $"Золото: {currentProfile.gold}";
        
        if (playerSpiritText != null)
            playerSpiritText.text = $"Дух: {currentProfile.spirit}";
        
        if (playerTitleText != null)
        {
            // Would lookup title name from database
            playerTitleText.text = "Титул: " + currentProfile.title_id;
        }
        
        // Update stat bars
        UpdateStatBars();
    }
    
    void UpdateStatBars()
    {
        float barWidth = 1f; // Would calculate based on actual stats
        
        if (strengthBar != null)
            strengthBar.transform.localScale = new Vector3(currentProfile.strength * 0.1f, 1f, 1f);
        
        if (agilityBar != null)
            agilityBar.transform.localScale = new Vector3(currentProfile.agility * 0.1f, 1f, 1f);
        
        if (intelligenceBar != null)
            intelligenceBar.transform.localScale = new Vector3(currentProfile.intelligence * 0.1f, 1f, 1f);
        
        if (staminaBar != null)
            staminaBar.transform.localScale = new Vector3(currentProfile.stamina * 0.1f, 1f, 1f);
        
        if (spiritBar != null)
            spiritBar.transform.localScale = new Vector3(currentProfile.spirit * 0.01f, 1f, 1f);
    }
    
    void SetupDropdowns()
    {
        // Setup hair styles
        if (hairStyleDropdown != null)
        {
            hairStyleDropdown.options.Clear();
            hairStyleDropdown.options.Add(new Dropdown.OptionData("Короткие"));
            hairStyleDropdown.options.Add(new Dropdown.OptionData("Длинные"));
            hairStyleDropdown.options.Add(new Dropdown.OptionData("Тедень"));
            hairStyleDropdown.options.Add(new Dropdown.OptionData("Леся"));
        }
        
        // Setup hair colors
        if (hairColorDropdown != null)
        {
            hairColorDropdown.options.Clear();
            hairColorDropdown.options.Add(new Dropdown.OptionData("Черные"));
            hairColorDropdown.options.Add(new Dropdown.OptionData("Кастаны"));
            hairColorDropdown.options.Add(new Dropdown.OptionData("Светлые"));
            hairColorDropdown.options.Add(new Dropdown.OptionData("Рыжие"));
        }
        
        // Setup skin colors
        if (skinColorDropdown != null)
        {
            skinColorDropdown.options.Clear();
            skinColorDropdown.options.Add(new Dropdown.OptionData("Светлая"));
            skinColorDropdown.options.Add(new Dropdown.OptionData("Средняя"));
            skinColorDropdown.options.Add(new Dropdown.OptionData("Темная"));
        }
    }
    
    public void SaveProfileChanges()
    {
        if (currentProfile != null)
        {
            housingMgr.UpdatePlayerProfile(playerGuid, currentProfile);
            Debug.Log("Player profile saved!");
        }
    }
}