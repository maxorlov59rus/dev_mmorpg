using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "NPCDatabase", menuName = "NPCs/NPCDatabase")]
public class NPCDatabase : ScriptableObject
{
    public List<NPCData> npcs;
    
    public NPCData GetNPC(int id)
    {
        return npcs.Find(x => x.id == id);
    }
    
    public List<NPCData> GetAllNPs()
    {
        return npcs;
    }
}