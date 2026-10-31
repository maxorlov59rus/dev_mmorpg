using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "FactionDatabase", menu = "Factions/FactionDatabase")]
public class FactionDatabase : ScriptableObject
{
    public List<FactionData> factions;
    
    public FactionData GetFaction(int id)
    {
        return factions.Find(x => x.id == id);
    }
    
    public List<FactionData> GetAllFactions()
    {
        return factions;
    }
}