using UnityEngine;

[CreateAssetMenu(fileName = "LoreEntry", menuName = "Lore/Entry")]
public class LoreEntry : ScriptableObject
{
    public int id;
    public string titleRu;
    public string titleEn;
    public string contentRu;
    public string contentEn;
    public string category; // lore, faction, quest, monster, item
}