using UnityEngine;
using UnityEngine.UI;

public class NPCWidget : MonoBehaviour
{
    public Text npcNameText;
    public Text npcTitleText;
    public Image npcImage;
    public Button interactButton;
    
    private NPCData currentNPC;
    
    void Start()
    {
        interactButton.onClick.AddListener(Interact);
    }
    
    public void ShowNPC(NPCData npc)
    {
        currentNPC = npc;
        npcNameText.text = npc.nameRu;
        npcTitleText.text = npc.titleRu;
        // npcImage.sprite = Resources.Load<Sprite>("NPCs/" + npc.id);
        gameObject.SetActive(true);
    }
    
    void Interact()
    {
        // ServerManager.InteractWithNPC(currentNPC.id);
        gameObject.SetActive(false);
    }
}