using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// ContaminationEntry - элемент зараженной зоны в списке
/// </summary>
public class ContaminationEntry : MonoBehaviour
{
    [Header("UI References")]
    public Text zoneName;
    public Text zoneLevel;
    public ProgressBar contaminationBar;
    public Button cureEntryButton;
    public GameObject curedIcon;
    
    private ContaminationZone zoneData;

    public void Initialize(ContaminationZone zone)
    {
        zoneData = zone;
        
        if (zoneName != null)
            zoneName.text = zone.name_ru;
        
        if (zoneLevel != null)
            zoneLevel.text = $"Уровень заразы: {zone.contamination_level}";
        
        if (contaminationBar != null)
            contaminationBar.value = zone.contamination_level;
        
        if (cureEntryButton != null)
        {
            cureEntryButton.onClick.RemoveAllListeners();
            cureEntryButton.onClick.AddListener(OnCureClicked);
        }
        
        if (curedIcon != null)
            curedIcon.SetActive(zone.contamination_level >= 80);
    }
    
    private void OnCureClicked()
    {
        // Request cure from server
        // Would send CMSG_CONTAMINATION_CURE packet
        Debug.Log($"Attempting to cure contamination in zone: {zoneData.name_ru}");
    }
}