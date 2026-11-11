using UnityEngine;

using UnityEngine.UI;
public class NPCDialogue50_60 : MonoBehaviour
{
    public Text dialogueText;
    public Text npcNameText;
    public Button acceptButton;
    public Button declineButton;
    
    private int currentNpcId;
    private int currentQuestId;
    
    void Start()
    {
        LoadDialogues();
    }
    
    void LoadDialogues()
    {
        TextAsset dialogue501 = Resources.Load<TextAsset>("Dialogues/Dialogue501");
        if (dialogue501 != null)
        {
            dialogueText.text = dialogue501.text;
        }
        
        TextAsset dialogue502 = Resources.Load<TextAsset>("Dialogues/Dialogue502");
        if (dialogue502 != null)
        {
            // Can switch between dialogues
        }
    }
    
    public void ShowDialogue(int npcId, int questId)
    {
        this.currentNpcId = npcId;
        this.currentQuestId = questId;
        gameObject.SetActive(true);
    }
    
    public void AcceptQuest()
    {
        // ServerManager.AcceptQuest(currentQuestId);
        gameObject.SetActive(false);
    }
    
    public void DeclineQuest()
    {
        gameObject.SetActive(false);
    }
}