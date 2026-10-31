using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

/// <summary>
/// ContaminationWidget - система заразы для игрока
/// </summary>
public class ContaminationWidget : MonoBehaviour
{
    [Header("UI References")]
    public Text contaminationLevelText;
    public Slider contaminationSlider;
    public Text debuffTypeText;
    public Text debuffValueText;
    public GameObject cureButton;
    public Text cureCostText;
    public Transform contaminationContent;
    public GameObject contaminationEntryTemplate;
    
    private ContaminationManager contaminationManager;
    private uint64 playerGuid;
    private List<ContaminationEntry> displayedZones = new List<ContaminationEntry>();
    
    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        contaminationManager = GameManager.Instance?.ContaminationManager;
        
        if (contaminationManager == null)
        {
            Debug.LogError("ContaminationManager not found!");
            enabled = false;
            return;
        }
        
        InitializeUI();
        LoadContaminationData();
        UpdateContaminationDisplay();
    }
    
    void InitializeUI()
    {
        contaminationSlider.onValueChanged.AddListener(OnContaminationSliderChanged);
        cureButton.onClick.AddListener(OnCureClicked);
        
        // Clear existing content
        if (contaminationContent != null)
        {
            foreach (Transform child in contaminationContent)
                Destroy(child.gameObject);
            displayedZones.Clear();
        }
    }
    
    void LoadContaminationData()
    {
        if (contaminationManager == null) return;
        
        var zones = contaminationManager.GetActiveZones();
        
        if (contaminationLevelText != null)
            contaminationLevelText.text = "Уровень заразы: --";
        
        if (contaminationSlider != null)
            contaminationSlider.value = 0;
        
        if (debuffTypeText != null)
            debuffTypeText.text = "--";
        if (debuffValueText != null)
            debuffValueText.text = "--";
        
        if (cureButton != null)
            cureButton.SetActive(false);
        
        // Create zone entries
        if (contaminationContent != null && zones != null)
        {
            foreach (var zone in zones)
            {
                var go = Instantiate(contaminationEntryTemplate, contaminationContent);
                go.SetActive(true);
                
                var entry = go.GetComponent<ContaminationEntry>();
                if (entry != null)
                {
                    entry.Initialize(zone);
                    displayedZones.Add(entry);
                }
            }
        }
    }
    
    void UpdateContaminationDisplay()
    {
        if (contaminationManager == null) return;
        
        var playerState = contaminationManager.GetPlayerContamination(playerGuid);
        if (playerState != null)
        {
            if (contaminationLevelText != null)
                contaminationLevelText.text = $"Уровень заразы: {playerState.infection_level}";
            
            if (contaminationSlider != null)
                contaminationSlider.value = playerState.infection_level;
            
            if (debuffTypeText != null && debuffValueText != null)
            {
                // Find appropriate debuff based on infection level
                float level = playerState.infection_level;
                string debuffType = "";
                float debuffValue = 0;
                
                if (level >= 80)
                {
                    debuffType = "Шанс крита";
                    debuffValue = -0.15f;
                }
                else if (level >= 50)
                {
                    debuffType = "Регенерация HP";
                    debuffValue = -0.1f;
                }
                else if (level >= 20)
                {
                    debuffType = "Сила";
                    debuffValue = -0.2f;
                }
                else
                {
                    debuffType = "Скорость";
                    debuffValue = -0.3f;
                }
                
                debuffTypeText.text = debuffType;
                debuffValueText.text = $"{debuffValue * 100}%";
            }
            
            if (cureButton != null)
                cureButton.SetActive(true);
            
            if (cureCostText != null)
                cureCostText.text = $"Антидот: {contaminationManager.GetCureCost()} золота";
        }
        else
        {
            if (contaminationLevelText != null)
                contaminationLevelText.text = "Уровень заразы: 0";
            
            if (contaminationSlider != null)
                contaminationSlider.value = 0;
            
            if (debuffTypeText != null)
                debuffTypeText.text = "--";
            if (debuffValueText != null)
                debuffValueText.text = "--";
            
            if (cureButton != null)
                cureButton.SetActive(false);
        }
    }
    
    void OnContaminationSliderChanged(float value)
    {
        // Player attempting to cure themselves with items
        contaminationManager.TryCurePlayer(playerGuid, (int)value);
    }
    
    void OnCureClicked()
    {
        if (contaminationManager == null) return;
        contaminationManager.TryCurePlayer(playerGuid, 100);
    }
}