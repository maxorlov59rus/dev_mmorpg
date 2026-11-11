using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "RaceDatabase", menuName = "Races/RaceDatabase")]
public class RaceDatabase : ScriptableObject
{
    public List<RaceData> races;
    
    public RaceData GetRace(int id)
    {
        return races.Find(x => x.id == id);
    }
    
    public List<RaceData> GetAllRaces()
    {
        return races;
    }
}