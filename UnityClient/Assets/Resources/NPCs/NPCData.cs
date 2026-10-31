using UnityEngine;

[CreateAssetMenu(fileName = "NPCData", menu = "NPCs/NPCData")]
public class NPCData : ScriptableObject
{
    public int id;
    public string nameRu;
    public string nameEn;
    public string titleRu;
    public string titleEn;
    public NPCType npcType;
    public int rank;
    public int factionId;
    public int cityId;
    public int gender;
    public string race;
    public string descriptionRu;
    public string descriptionEn;
    public bool isQuestGiver;
    public bool isTrainer;
    public bool isSpecialInteract;
}

public enum NPCType
{
    QuestGiver,
    Trainer,
    Merchant,
    SpecialInteract,
    Monster
}