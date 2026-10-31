using UnityEngine;
using UnityEngine.UI;

public class FactionWidget : MonoBehaviour
{
    public Text factionNameText;
    public Image factionColorImage;
    public Text factionDescriptionText;
    public Button joinFactionButton;
    
    private FactionData currentFaction;
    
    void Start()
    {
        joinFactionButton.onClick.AddListener(JoinFaction);
    }
    
    public void ShowFaction(FactionData faction)
    {
        currentFaction = faction;
        factionNameText.text = faction.nameRu;
        factionColorImage.color = faction.factionColor;
        factionDescriptionText.text = faction.descriptionRu;
        gameObject.SetActive(true);
    }
    
    void JoinFaction()
    {
        // ServerManager.JoinFaction(currentFaction.id);
        gameObject.SetActive(false);
    }
}