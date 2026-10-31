using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "LoreDatabase", menu = "Lore/Database")]
public class LoreDatabase : ScriptableObject
{
    public List<LoreEntry> entries;
    
    public LoreEntry GetEntry(int id)
    {
        return entries.Find(x => x.id == id);
    }
    
    public List<LoreEntry> GetEntriesByCategory(string category)
    {
        return entries.FindAll(x => x.category == category);
    }
}