using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

/// <summary>
/// QuestLogWidget - квестовый журнал
/// </summary>
public class QuestLogWidget : MonoBehaviour
{
    [Header("UI References")]
    public Text questLogTitle;
    public Transform questContent;
    public GameObject questTemplate;
    public Button closeButton;
    public Text emptyStateText;
    
    private uint64 playerGuid;
    private QuestManager questManager;
    private List<QuestLogEntry> displayedQuests = new List<QuestLogEntry>();
    
    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        questManager = GameManager.Instance?.QuestManager;
        
        if (questManager == null)
        {
            Debug.LogError("QuestManager not found!");
            enabled = false;
            return;
        }
        
        InitializeUI();
        LoadQuests();
    }
    
    void InitializeUI()
    {
        closeButton.onClick.AddListener(CloseWindow);
        
        if (questTemplate != null)
            questTemplate.SetActive(false);
        
        if (questContent != null)
        {
            foreach (Transform child in questContent)
                Destroy(child.gameObject);
            displayedQuests.Clear();
        }
    }
    
    void LoadQuests()
    {
        if (questManager == null) return;
        
        var quests = questManager.GetAllQuests();
        
        if (questLogTitle != null)
            questLogTitle.text = "Квестовый журнал";
        
        if (questContent != null)
        {
            foreach (var quest in quests)
            {
                var go = Instantiate(questTemplate, questContent);
                go.SetActive(true);
                
                var entry = go.GetComponent<QuestLogEntry>();
                if (entry != null)
                {
                    entry.Initialize(quest);
                    displayedQuests.Add(entry);
                }
            }
        }
        
        // Show/hide empty state
        if (emptyStateText != null)
            emptyStateText.SetActive(displayedQuests.Count == 0);
    }
    
    public void OpenWindow(uint64 guid)
    {
        playerGuid = guid;
        gameObject.SetActive(true);
        LoadQuests();
    }
    
    public void CloseWindow()
    {
        gameObject.SetActive(false);
    }
}