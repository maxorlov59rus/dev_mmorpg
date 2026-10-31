using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// Quest Giver Widget - Interface for accepting quests from NPCs
/// </summary>
public class QuestGiverWidget : MonoBehaviour
{
    [Header("UI References")]
    [SerializeField] private Button closeButton;
    [SerializeField] private Text npcNameText;
    [SerializeField] private Text questTitleText;
    [SerializeField] private Text questDescriptionText;
    [SerializeField] private Text levelRequirementText;
    [SerializeField] private Text rewardText;
    [SerializeField] private Button acceptQuestButton;
    [SerializeField] private Transform questContentPanel;
    [SerializeField] private GameObject questEntryPrefab;
    
    [Header("Settings")]
    [SerializeField] private int minPlayerLevelToAccept = 70;
    
    // Cached data
    private LoreMgr loreManager;
    private NpcInfo currentNpc;
    private QuestInfo currentQuest;
    private List<QuestInfo> availableQuests = new List<QuestInfo>();
    
    void Awake()
    {
        // Initialize UI references
        InitializeUI();
    }
    
    void Start()
    {
        // Find lore manager
        loreManager = FindObjectOfType<LoreMgr>();
        if (loreManager == null)
        {
            Debug.LogError("LoreMgr not found in scene!");
            enabled = false;
            return;
        }
    }
    
    void Update()
    {
        // Update level requirement text if we have a quest
        if (currentQuest != null)
        {
            levelRequirementText.text = "Required Level: " + currentQuest.level_min + "+";
        }
    }
    
    void InitializeUI()
    {
        // Setup close button
        if (closeButton != null)
        {
            closeButton.onClick.AddListener(CloseWidget);
        }
        
        // Setup accept quest button
        if (acceptQuestButton != null)
        {
            acceptQuestButton.onClick.AddListener(AcceptQuest);
        }
        
        // Setup content panel
        if (questContentPanel != null)
        {
            questContentPanel.gameObject.SetActive(true);
        }
    }
    
    /// <summary>
    /// Open quest giver interface for specific NPC
    /// </summary>
    /// <param name="npcId">NPC ID</param>
    public void OpenWidget(int npcId)
    {
        // Find NPC info
        currentNpc = loreManager.getNpcById(npcId);
        if (currentNpc == null)
        {
            Debug.LogError("NPC not found: " + npcId);
            return;
        }
        
        // Update NPC name
        if (npcNameText != null)
        {
            npcNameText.text = currentNpc.name_ru + " (" + currentNpc.title_ru + ")";
        }
        
        // Get available quests for this NPC and player level
        availableQuests = loreManager.getQuestsByNpc(npcId);
        // Filter by player level - use minimum player level (assume 70 for now)
        availableQuests = loreManager.getAvailableQuests(70, currentNpc.faction_id);
        
        // Update UI
        UpdateQuestDisplay();
        
        // Show widget
        gameObject.SetActive(true);
    }
    
    void UpdateQuestDisplay()
    {
        // Clear existing quest entries
        if (questContentPanel != null)
        {
            foreach (Transform child in questContentPanel)
            {
                if (child.name.StartsWith("QuestEntry_"))
                    Destroy(child.gameObject);
            }
        }
        
        // Add available quest entries
        foreach (var quest in availableQuests)
        {
            GameObject entry = Instantiate(questEntryPrefab, questContentPanel);
            entry.name = "QuestEntry_" + quest.id;
            QuestEntry entryScript = entry.GetComponent<QuestEntry>();
            if (entryScript != null)
            {
                entryScript.quest = quest;
                entryScript.loreManager = loreManager;
                entryScript.onQuestAccept += OnQuestAccepted;
            }
        }
        
        // Update quest title and description if we have a current quest
        if (currentQuest != null && questTitleText != null)
        {
            questTitleText.text = currentQuest.title_ru;
        }
        if (currentQuest != null && questDescriptionText != null)
        {
            questDescriptionText.text = currentQuest.description_ru;
        }
        if (currentQuest != null && levelRequirementText != null)
        {
            levelRequirementText.text = "Required Level: " + currentQuest.level_min + "+";
        }
        if (currentQuest != null && rewardText != null)
        {
            rewardText.text = "Reward: " + currentQuest.reward_value + " " + currentQuest.reward_type;
        }
    }
    
    void AcceptQuest()
    {
        if (currentQuest != null)
        {
            // In a real implementation, we would send a packet to the server
            // to accept the quest and add it to the player's quest log
            Debug.Log("Accepted quest: " + currentQuest.title_ru);
            CloseWidget();
        }
    }
    
    void OnQuestAccepted(QuestInfo quest)
    {
        currentQuest = quest;
        UpdateQuestDisplay();
    }
    
    void CloseWidget()
    {
        gameObject.SetActive(false);
    }
}