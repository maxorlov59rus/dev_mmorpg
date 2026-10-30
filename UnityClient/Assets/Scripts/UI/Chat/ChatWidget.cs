using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// Chat Widget - main chat interface
/// </summary>
public class ChatWidget : MonoBehaviour
{
    [Header("UI References")]
    public InputField messageInput;
    public Button sendButton;
    public Transform chatContent;
    public GameObject messageTemplate;
    public Toggle globalToggle;
    public Toggle regionToggle;
    public Toggle factionToggle;
    public Toggle partyToggle;
    public Toggle whisperToggle;
    public InputField whisperTargetInput;
    
    [Header("Settings")]
    public Text channelDisplay;
    public Button closeButton;
    
    private uint64 playerGuid;
    private ChatManager chatManager;
    private string currentChannelType = "global";
    private uint64 currentChannelId = 0;
    private bool isWhisperTargetSet = false;

    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        chatManager = GameManager.Instance?.ChatManager;
        
        if (chatManager == null)
        {
            Debug.LogError("ChatManager not found!");
            enabled = false;
            return;
        }

        InitializeUI();
        LoadChatHistory();
        UpdateChannelDisplay();
    }

    void InitializeUI()
    {
        // Setup send button
        sendButton.onClick.AddListener(SendMessage);

        // Setup close button
        closeButton.onClick.AddListener(CloseWindow);

        // Setup toggles
        globalToggle.onClick.AddListener(() => SetChannelType("global"));
        regionToggle.onClick.AddListener(() => SetChannelType("region"));
        factionToggle.onClick.AddListener(() => SetChannelType("faction"));
        partyToggle.onClick.AddListener(() => SetChannelType("party"));
        whisperToggle.onClick.AddListener(() => SetChannelType("whisper"));

        // Initially select global
        SetChannelType("global");

        // Hide whisper target input initially
        whisperTargetInput.gameObject.SetActive(false);
    }

    void SetChannelType(string type)
    {
        currentChannelType = type;
        
        // Update toggle states
        globalToggle.isOn = type == "global";
        regionToggle.isOn = type == "region";
        factionToggle.isOn = type == "faction";
        partyToggle.isOn = type == "party";
        whisperToggle.isOn = type == "whisper";

        // Show/hide whisper target
        whisperTargetInput.gameObject.SetActive(type == "whisper");
        channelDisplay.text = $"Чат: {type.ToUpper()}";
        
        LoadChatHistory();
    }

    void SendMessage()
    {
        if (string.IsNullOrWhiteSpace(messageInput.text))
            return;

        string text = messageInput.text;
        messageInput.text = "";

        uint64 targetGuid = 0;
        if (currentChannelType == "whisper" && !string.IsNullOrWhiteSpace(whisperTargetInput.text))
        {
            // Would look up player GUID from name
            targetGuid = 0; // Placeholder
            isWhisperTargetSet = true;
        }

        chatManager.SendMessage(playerGuid, text, currentChannelType, targetGuid);
        
        // Add local message to UI
        AddMessageToUI(playerGuid, text, currentChannelType, false);
    }

    void LoadChatHistory()
    {
        if (chatManager == null) return;

        // Clear existing messages
        foreach (Transform child in chatContent)
            Destroy(child.gameObject);

        // Load global chat history
        if (currentChannelType == "global" || currentChannelType == "whisper")
        {
            var history = chatManager.GetMessageHistory(0, 50);
            foreach (var msg in history)
            {
                AddMessageToUI(msg.playerGuid, msg.text, msg.isWhisper ? "whisper" : "global", msg.isWhisper);
            }
        }
        else
        {
            var history = chatManager.GetMessageHistory(currentChannelId > 0 ? currentChannelId : 0, 50);
            foreach (var msg in history)
            {
                AddMessageToUI(msg.playerGuid, msg.text, currentChannelType, msg.isWhisper);
            }
        }
    }

    void AddMessageToUI(uint64 playerGuid, string text, string channelType, bool isWhisper)
    {
        var go = Instantiate(messageTemplate, chatContent);
        go.SetActive(true);

        var entry = go.GetComponent<ChatMessageEntry>();
        if (entry == null) entry = go.AddComponent<ChatMessageEntry>();

        entry.Initialize(playerGuid, text, channelType, isWhisper);
    }

    public void OpenWindow(uint64 guid)
    {
        playerGuid = guid;
        gameObject.SetActive(true);
        LoadChatHistory();
    }

    public void CloseWindow()
    {
        gameObject.SetActive(false);
    }
}

/// <summary>
/// Individual chat message entry
/// </summary>
public class ChatMessageEntry : MonoBehaviour
{
    [Header("UI References")]
    public Text playerName;
    public Text messageText;
    public Text channelType;
    public GameObject whisperIcon;

    public void Initialize(uint64 playerGuid, string text, string channelType, bool isWhisper)
    {
        // Would look up player name from GUID
        if (playerName != null)
            playerName.text = "Игрок"; // Placeholder

        if (messageText != null)
            messageText.text = text;

        if (channelType != null)
            this.channelType.text = channelType;

        if (whisperIcon != null)
            whisperIcon.SetActive(isWhisper);
    }
}