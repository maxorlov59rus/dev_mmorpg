using UnityEngine;
using UnityEngine.UI;

namespace BlackMateria.UI.Guilds
{
    /// <summary>
    /// Guild War widget displaying active wars and objectives
    /// </summary>
    public class GuildWarWidget : MonoBehaviour
    {
        [Header("War List")]
        [SerializeField] private GameObject warItemPrefab;
        [SerializeField] private Transform warListContainer;
        [SerializeField] private Button declareWarButton;
        
        [Header("Alliance Controls")]
        [SerializeField] private InputField allianceNameInput;
        [SerializeField] private Button createAllianceButton;
        [SerializeField] private Button dissolveAllianceButton;
        
        private void Start()
        {
            UpdateWarList();
            declareWarButton.onClick.AddListener(OnDeclareWar);
            createAllianceButton.onClick.AddListener(OnCreateAlliance);
            dissolveAllianceButton.onClick.AddListener(OnDissolveAlliance);
        }
        
        private void UpdateWarList()
        {
            // Clear existing items
            foreach (Transform child in warListContainer)
            {
                Destroy(child.gameObject);
            }
            
            // Get active wars
            var wars = GuildWarMgr.Instance.GetActiveWars();
            
            foreach (var war in wars)
            {
                // Create war item
                GameObject item = Instantiate(warItemPrefab, warListContainer);
                // Set war data on the item
                // warItem.Setup(war);
            }
        }
        
        private void OnDeclareWar()
        {
            // Get guild IDs from UI
            // int guildAId = ...;
            // int guildBId = ...;
            // string reason = ...;
            
            // GuildWarMgr.Instance.DeclareWar(guildAId, guildBId, reason);
        }
        
        private void OnCreateAlliance()
        {
            // Get guild IDs and alliance type
            // int guildAId = ...;
            // int guildBId = ...;
            // string allianceType = allianceNameInput.text;
            
            // GuildWarMgr.Instance.CreateAlliance(guildAId, guildBId, allianceType);
        }
        
        private void OnDissolveAlliance()
        {
            // Get guild IDs
            // int guildAId = ...;
            // int guildBId = ...;
            
            // GuildWarMgr.Instance.DissolveAlliance(guildAId, guildBId);
        }
    }
}