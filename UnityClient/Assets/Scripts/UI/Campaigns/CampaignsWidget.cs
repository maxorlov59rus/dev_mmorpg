using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

/// <summary>
/// Campaigns Widget - система кампаний уровня 70+
/// </summary>
public class CampaignsWidget : MonoBehaviour
{
    [Header("UI References")]
    public Transform campaignsContent;
    public GameObject campaignTemplate;
    public Button createCampaignButton;
    public Text campaignsTitle;
    public Text requirementsText;
    public Text rewardText;
    
    private uint64 playerGuid;
    private CampaignManager campaignManager;
    private List<CampaignData> displayedCampaigns = new List<CampaignData>();

    void Awake()
    {
        playerGuid = GameManager.Instance?.PlayerGuid ?? 0;
        campaignManager = GameManager.Instance?.CampaignManager;
        
        if (campaignManager == null)
        {
            Debug.LogError("CampaignManager not found!");
            enabled = false;
            return;
        }

        InitializeUI();
        LoadCampaigns();
    }

    void InitializeUI()
    {
        // Setup template - hide by default
        if (campaignTemplate != null)
            campaignTemplate.SetActive(false);

        // Update title
        if (campaignsTitle != null)
            campaignsTitle.text = "Кампании 70+";

        // Clear existing content
        if (campaignsContent != null)
        {
            foreach (Transform child in campaignsContent)
                Destroy(child.gameObject);
            displayedCampaigns.Clear();
        }

        // Load and display campaigns
        LoadCampaigns();
    }

    void LoadCampaigns()
    {
        var campaigns = campaignManager.GetActiveCampaigns();
        
        foreach (var campaign in campaigns)
        {
            var go = Instantiate(campaignTemplate, campaignsContent);
            go.SetActive(true);
            
            var data = go.AddComponent<CampaignEntry>();
            data.Initialize(campaign);
            displayedCampaigns.Add(data);
        }
    }

    void OnCampaignSelected(uint32 campaignId)
    {
        // Open campaign details or start campaign
        Debug.Log($"Starting campaign: {campaignId}");
    }
}