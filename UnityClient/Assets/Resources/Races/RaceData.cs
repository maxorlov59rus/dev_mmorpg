using UnityEngine;

[CreateAssetMenu(fileName = "RaceData", menu = "Races/RaceData")]
public class RaceData : ScriptableObject
{
    public int id;
    public string nameRu;
    public string nameEn;
    public string descriptionRu;
    public string descriptionEn;
    public float movementSpeed;
    public float manaRegen;
    public int strength;
    public int agility;
    public int intelligence;
    public int stamina;
    public int spirit;
}