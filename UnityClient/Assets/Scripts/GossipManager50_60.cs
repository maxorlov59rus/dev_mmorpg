using UnityEngine;

public class GossipManager50_60 : MonoBehaviour
{
    public Text gossipText;
    
    void Start()
    {
        LoadGossips();
    }
    
    void LoadGossips()
    {
        TextAsset gossip601 = Resources.Load<TextAsset>("Gossips/Gossip601");
        if (gossip601 != null)
        {
            gossipText.text = gossip601.text;
        }
    }
    
    public void ShowGossip()
    {
        gameObject.SetActive(true);
    }
    
    public void AcceptGossip()
    {
        // ServerManager.AcceptQuest(101);
        gameObject.SetActive(false);
    }
    
    public void DeclineGossip()
    {
        gameObject.SetActive(false);
    }
}