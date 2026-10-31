using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// NPC Dialogue Widget - Interface for displaying and interacting with NPC dialogues
/// </summary>
public class NPCDialogueWidget : MonoBehaviour
{
    [Header("UI References")]
    [SerializeField] private Button closeButton;
    [SerializeField] private Text npcNameText;
    [SerializeField] private Text dialogueText;
    [SerializeField] private Text emotionText;
    [SerializeField] private Button[] dialogueButtons;
    [SerializeField] private Transform dialogueContentPanel;
    [SerializeField] private GameObject dialogueEntryPrefab;
    
    [Header("Settings")]
    [SerializeField] private float dialogueDisplayTime = 5f;
    
    // Cached data
    private LoreMgr loreManager;
    private NpcInfo currentNpc;
    private int currentDialogueIndex = 0;
    private float displayTimer = 0f;
    
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
        // Update dialogue timer
        if (displayTimer > 0f)
        {
            displayTimer -= Time.deltaTime;
            if (displayTimer <= 0f)
            {
                CloseWidget();
            }
        }
    }
    
    void InitializeUI()
    {
        // Setup close button
        if (closeButton != null)
        {
            closeButton.onClick.AddListener(CloseWidget);
        }
        
        // Setup dialogue buttons
        for (int i = 0; i < dialogueButtons.Length; i++)
        {
            int buttonIndex = i;
            dialogueButtons[i].onClick.AddListener(() => OnDialogueChoice(buttonIndex));
        }
        
        // Setup content panel
        if (dialogueContentPanel != null)
        {
            dialogueContentPanel.gameObject.SetActive(true);
        }
    }
    
    /// <summary>
    /// Open dialogue with specific NPC
    /// </summary>
    /// <param name="npcId">NPC ID</param>
    /// <param name="dialogueType">Type of dialogue