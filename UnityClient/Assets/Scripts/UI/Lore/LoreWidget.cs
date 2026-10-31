using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// World Lore Widget - Main interface for browsing cities, NPCs, and quests
/// </summary>
public class LoreWidget : MonoBehaviour
{
    [Header("UI References")]
    [SerializeField] private Button closeButton;
    [SerializeField] private Transform contentPanel;
    [SerializeField] private GameObject cityEntryPrefab;
    [SerializeField] private GameObject npcEntryPrefab;
    [SerializeField] private GameObject questEntryPrefab;
    
    [Header("Settings")]
    [SerializeField] private int minPlayerLevel = 70;
    
    // Cached data
    private LoreMgr loreManager;
    private List<CityInfo> displayedCities = new List<CityInfo>();
    private List<NpcInfo> displayedNPCs = new List<NpcInfo>();
    private List<QuestInfo> displayedQuests = new List<QuestInfo>();
    
    void Awake()
    {
        // Initialize lore manager
        loreManager = FindObjectOfType<LoreMgr>();
        if (loreManager == null)
        {
            Debug.LogError("LoreMgr not found in scene!");
            return;
        }
        
        // Initialize UI
        InitializeUI();
    }
    
    void Start()
    {
        LoadLoreData();
    }
    
    void InitializeUI()
    {
        // Setup close button
        if (closeButton != null)
        {
            closeButton.onClick.AddListener(CloseWidget);
        }
        
        // Setup content panel
        if (contentPanel != null)
        {
            contentPanel.gameObject.SetActive(true);
        }
    }
    
    void LoadLoreData()
    {
        if (loreManager == null) return;
        
        // Load and display cities
        displayedCities = loreManager.getAllCities();
        DisplayCities();
        
        // Initially show first city's NPCs and quests
        if (displayedCities.Count > 0)
        {
            DisplayNPCsForCity(displayedCities[0].id);
            DisplayQuestsForCity(displayedCities[0].id);
        }
    }
    
    void DisplayCities()
    {
        // Clear existing entries
        foreach (Transform child in contentPanel)
        {
            if (child.name.StartsWith("CityEntry_"))
                Destroy(child.gameObject);
        }
        displayedCities.Clear();
        
        // Add city entries
        foreach (var city in displayedCities)
        {
            GameObject entry = Instantiate(cityEntryPrefab, contentPanel);
            entry.name = "CityEntry_" + city.id;
            CityEntry entryScript = entry.GetComponent<CityEntry>();
            if (entryScript != null)
            {
                entryScript.city = city;
                entryScript.loreManager = loreManager;
                entryScript.onCitySelected += OnCitySelected;
            }
        }
    }
    
    void DisplayNPCsForCity(int cityId)
    {
        // Clear existing NPC entries
        foreach (Transform child in contentPanel)
        {
            if (child.name.StartsWith("NPCEntry_"))
                Destroy(child.gameObject);
        }
        displayedNPCs.Clear();
        
        // Get NPCs for this city
        var npcs = loreManager.getNpcsByCity(cityId);
        displayedNPCs = npcs;
        
        // Add NPC entries
        foreach (var npc in npcs)
        {
            GameObject entry = Instantiate(npcEntryPrefab, contentPanel);
            entry.name = "NPCEntry_" + npc.id;
            NpcEntry entryScript = entry.GetComponent<NpcEntry>();
            if (entryScript != null)
            {
                entryScript.npc = npc;
                entryScript.loreManager = loreManager;
                entryScript.onNpcSelected += OnNpcSelected;
            }
        }
    }
    
    void DisplayQuestsForCity(int cityId)
    {
        // Clear existing quest entries
        foreach (Transform child in contentPanel)
        {
            if (child.name.StartsWith("QuestEntry_"))
                Destroy(child.gameObject);
        }
        displayedQuests.Clear();
        
        // Get quests for this city
        var quests = loreManager.getQuestsByCity(cityId);
        displayedQuests = quests;
        
        // Add quest entries
        foreach (var quest in quests)
        {
            GameObject entry = Instantiate(questEntryPrefab, contentPanel);
            entry.name = "QuestEntry_" + quest.id;
            QuestEntry entryScript = entry.GetComponent<QuestEntry>();
            if (entryScript != null)
            {
                entryScript.quest = quest;
                entryScript.loreManager = loreManager;
                entryScript.onQuestSelected += OnQuestSelected;
            }
        }
    }
    
    void OnCitySelected(CityInfo city)
    {
        // Update UI to show NPCs and quests for this city
        DisplayNPCsForCity(city.id);
        DisplayQuestsForCity(city.id);
    }
    
    void OnNpcSelected(NpcInfo npc)
    {
        // Open dialogue or show NPC details
        Debug.Log("Selected NPC: " + npc.name_ru + " - " + npc.title_ru);
    }
    
    void OnQuestSelected(QuestInfo quest)
    {
        // Open quest details or add to quest log
        Debug.Log("Selected Quest: " + quest.title_ru + " (Lvl " + quest.level_min + "-" + quest.level_max + ")");
    }
    
    void CloseWidget()
    {
        gameObject.SetActive(false);
    }
}