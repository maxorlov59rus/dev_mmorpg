using UnityEngine;
using System.Collections.Generic;

[CreateAssetMenu(fileName = "ClassDatabase", menu = "Classes/ClassDatabase")]
public class ClassDatabase : ScriptableObject
{
    public List<ClassData> classes;
    
    public ClassData GetClass(int id)
    {
        return classes.Find(x => x.id == id);
    }
    
    public List<ClassData> GetAllClasses()
    {
        return classes;
    }
}