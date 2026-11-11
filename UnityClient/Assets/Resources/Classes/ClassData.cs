using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "ClassData", menuName = "Classes/ClassData")]
public class ClassData : ScriptableObject
{
    public int id;
    public string nameRu;
    public string nameEn;
    public string descriptionRu;
    public string descriptionEn;
    public string primaryStat; // strength, agility, intelligence, stamina, spirit
    public int baseHealth;
    public int baseMana;
    public float attackPower;
    public float defense;
    public float castSpeed;
    public List<string> availableRoles; // tank, dps, healer, support
}