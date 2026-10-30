using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Individual campaign entry in the campaigns list
/// </summary>
public class CampaignEntry : MonoBehaviour
{
    [Header("UI References")]
    public Text campaignName;
    public Text levelRange;
    public Text difficultyText;
    public Text rewardText;
    public Button startButton;
    public GameObject lockIcon;
    public Text requiredPowerText;

    private CampaignDef campaignDef;

    public void Initialize(CampaignDef def)
    {
        campaignDef = def;

        if (campaignName != null)
            campaignName.text = def.name_ru;

        if (levelRange != null)
            levelRange.text = $"Уровни: {def.level_start}-{def.level_end}";

        if (difficultyText != null)
        {
            string diffStr = "";
            for (int i = 0; i < def.difficulty; i++)
                diffStr += "★ ";
            difficultyText.text = diffStr;
        }

        if (rewardText != null)
        {
            // Show approximate reward
            rewardText.text = $"Награда: {def.estimated_duration_minutes} мин";
        }

        // Setup start button
        if (startButton != null)
        {
            startButton.onClick.RemoveAllListeners();
            startButton.onClick.AddListener(OnStartClicked);
        }

        // Check if player can access this campaign
        CheckAccess();
    }

    void CheckAccess()
    {
        // Check player level and requirements
        // Would check player level vs campaign level range
        // Would check if previous campaigns are completed
        
        // For now, just show lock if campaign is too high level
        // lockIcon.SetActive(false); // Would check player level
    }

    private void OnStartClicked()
    {
        // Open campaign or show requirements not met
        Debug.Log($"Starting campaign: {campaignDef.name_ru}");
    }
}