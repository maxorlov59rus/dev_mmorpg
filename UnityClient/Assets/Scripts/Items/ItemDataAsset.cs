// ItemDataAsset.cs — Unity ScriptableObject для предметов
using UnityEngine;

namespace DarkMatter.Items
{
    [CreateAssetMenu(fileName = "DA_Item", menuName = "DarkMatter/ItemDataAsset")]
    public class ItemDataAsset : ScriptableObject
    {
        public int Entry;
        public string Name;
        public int ItemLevel;
        public int Quality; // 1=Common, 2=Uncommon, 3=Rare, 4=Epic, 5=Legendary
        public string MeshPath;
        public string IconPath;
        public int DisplayId;
    }
}
