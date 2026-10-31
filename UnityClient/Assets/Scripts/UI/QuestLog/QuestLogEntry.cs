using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// QuestLogEntry - элемент квеста в журнале
/// </summary>
public class QuestLogEntry : MonoBehaviour
{
    [Header("UI References")]
    public Text questName;
    public Text questDescription;
    public Text questProgress;
    public Button acceptButton;
    public GameObject completedMark;
    
    private QuestData questData;

    public void Initialize(QuestData data)
    {
        questData = data;
        
        if (questName != null)
            questName.text = data.questName;
        
        if (questDescription != null)
            questDescription.text = data.questDescription;
        
        if (questProgress != null)
            questProgress.text = $"Прогресс: {questData.currentProgress}/{questData.targetProgress}";
        
        if (acceptButton != null)
        {
            acceptButton.onClick.RemoveAllListeners();
            acceptButton.onClick.AddListener(AcceptQuest);
        }
        
        if (completedMark != null)
            completedMark.SetActive(questData.isCompleted);
    }
    
    private void AcceptQuest()
    {
        // Request quest acceptance from server
        // Would send CMSG_ACCEPT_QUEST packet
        Debug.Log($"Accepting quest: {questData.questName}");
    }
}