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
    private NpcInfo currentNpc;
    private int currentDialogueIndex = 0;
    private float displayTimer = 0f;

    void Awake()
    {
        InitializeUI();
    }

    void Start()
    {
        ShowDialogues();
    }

    void Update()
    {
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
        if (closeButton != null)
        {
            closeButton.onClick.AddListener(CloseWidget);
        }

        if (dialogueButtons != null)
        {
            for (int i = 0; i < dialogueButtons.Length; i++)
            {
                int buttonIndex = i;
                if (dialogueButtons[i] != null)
                {
                    dialogueButtons[i].onClick.AddListener(() => OnDialogueChoice(buttonIndex));
                }
            }
        }

        if (dialogueContentPanel != null)
        {
            dialogueContentPanel.gameObject.SetActive(true);
        }
    }

    /// <summary>
    /// Open dialogue with specific NPC
    /// </summary>
    public void OpenDialogue(NpcInfo npc, float? displayTime = null)
    {
        currentNpc = npc;
        currentDialogueIndex = 0;
        displayTimer = displayTime ?? dialogueDisplayTime;

        gameObject.SetActive(true);

        if (npcNameText != null && npc != null)
            npcNameText.text = npc.name;

        ShowDialogues();
    }

    private void ShowDialogues()
    {
        if (dialogueText == null || currentNpc == null)
            return;

        if (currentNpc.dialogues == null || currentNpc.dialogues.Count == 0)
        {
            dialogueText.text = "...";
            return;
        }

        if (currentDialogueIndex < 0 || currentDialogueIndex >= currentNpc.dialogues.Count)
            currentDialogueIndex = 0;

        dialogueText.text = currentNpc.dialogues[currentDialogueIndex];

        if (emotionText != null)
            emotionText.text = currentNpc.emotion;
    }

    private void OnDialogueChoice(int index)
    {
        if (currentNpc == null || currentNpc.dialogues == null)
            return;

        currentDialogueIndex = index;
        displayTimer = dialogueDisplayTime;
        ShowDialogues();
    }

    public void CloseWidget()
    {
        displayTimer = 0f;
        gameObject.SetActive(false);
    }
}
