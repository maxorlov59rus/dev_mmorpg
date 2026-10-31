using UnityEngine;
using UnityEngine.UI;

public class RaceWidget : MonoBehaviour
{
    public Text raceNameText;
    public Text raceDescriptionText;
    public Image raceColorImage;
    public Button selectRaceButton;
    
    private RaceData currentRace;
    
    void Start()
    {
        selectRaceButton.onClick.AddListener(SelectRace);
    }
    
    public void ShowRace(RaceData race)
    {
        currentRace = race;
        raceNameText.text = race.nameRu;
        raceColorImage.color = race.factionColor; // Note: RaceData doesn't have factionColor, need to fix
        raceDescriptionText.text = race.descriptionRu;
        gameObject.SetActive(true);
    }
    
    void SelectRace()
    {
        // ServerManager.SelectRace(currentRace.id);
        gameObject.SetActive(false);
    }
}