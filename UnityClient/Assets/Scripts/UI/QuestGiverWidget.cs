using UnityEngine;
using UnityEngine.UI;

public class QuestGiverWidget : MonoBehaviour
{
    public Text npcNameText;
    public Text questTitleText;
    public Text questDescriptionText;
    public Text rewardText;
    public GameObject questPrefab;
    public Transform contentPanel;
    
    private NPCBase currentNpc;
    private List<Quest50_60Data> availableQuests;
    private Quest50_60Data currentQuest;
    
    void Start()
    {
        availableQuests = new List<Quest50_60Data>();
        LoadQuests50_60();
    }
    
    void LoadQuests50_60()
    {
        Quest50_60Data quest101 = Resources.Load<Quest50_60Data>("Quests/Quest101");
        Quest50_60Data quest102 = Resources.Load<Quest50_60Data>("Quests/Quest102");
        
        if (quest101 != null)
        {
            availableQuests.Add(quest101);
            InstantiateQuestWidget(quest101);
        }
        
        if (quest102 != null)
        {
            availableQuests.Add(quest102);
            InstantiateQuestWidget(quest102);
        }
    }
    
    void InstantiateQuestWidget(Quest50_60Data questData)
    {
        GameObject widget = Instantiate(prefab, contentPanel);
        widget.GetComponentInChildren<Text>().text = questData.titleRu;
        
        widget.GetComponent<Button>().onClick.AddListener(() => OnQuestSelected(questData));
    }
    
    void OnQuestSelected(Quest50_60Data questData)
    {
        currentQuest = questData;
        questTitleText.text = questData.titleRu;
        questDescriptionText.text = questData.descriptionRu;
        
        if (questData.rewardType == "gold")
            rewardText.text = "+" + questData.rewardValue + " золота";
        else if (questData.rewardType == "spirit")
            rewardText.text = "+" + questData.rewardValue + " духа";
        
        gameObject.SetActive(true);
    }
}