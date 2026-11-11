using UnityEngine;

[CreateAssetMenu(fileName = "FactionData", menuName = "Factions/FactionData")]
public class FactionData : ScriptableObject
{
    public int id;
    public string nameRu;
    public string nameEn;
    public string descriptionRu;
    public string descriptionEn;
    public Color factionColor;
    public Sprite factionSymbol;
}