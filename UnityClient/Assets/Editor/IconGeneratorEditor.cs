// IconGeneratorEditor.cs
using UnityEngine;
using UnityEditor;
using System.IO;

namespace DM.Editor
{
    public class IconGeneratorEditor : EditorWindow
    {
        [MenuItem("Tools/Icons/Generate Placeholder Icons")]
        public static void GeneratePlaceholderIcons()
        {
            string basePath = "Assets/Icons";
            string[] categories = { "Skills", "Passive", "Weapons", "Armor", "Consumable", "Accessories", "Engineering", "Misc", "Classes", "Spirits", "Professions", "Quality" };
            foreach (var cat in categories)
            {
                string path = Path.Combine(basePath, cat);
                if (!Directory.Exists(path)) Directory.CreateDirectory(path);
            }
            GenerateIconSet("Skills", new string[] { "shield_bash", "taunt", "stone_skin", "heal", "blessing", "purify", "fireball", "frost_bolt", "chain_lightning", "battle_cry", "slow", "party_shield", "backstab", "shadow_step", "aimed_shot", "multishot", "shadow_bolt", "drain_life" }, new Color[] { new Color(0.8f,0.4f,0.2f), new Color(0.9f,0.2f,0.2f), new Color(0.6f,0.6f,0.6f), new Color(0.2f,0.8f,0.2f), new Color(0.9f,0.9f,0.2f), new Color(0.2f,0.6f,0.9f), new Color(0.9f,0.3f,0.1f), new Color(0.3f,0.5f,0.9f), new Color(0.5f,0.8f,0.2f), new Color(0.9f,0.7f,0.1f), new Color(0.4f,0.2f,0.8f), new Color(0.2f,0.8f,0.8f), new Color(0.3f,0.3f,0.3f), new Color(0.5f,0.2f,0.7f), new Color(0.8f,0.6f,0.2f), new Color(0.2f,0.7f,0.3f), new Color(0.4f,0.1f,0.6f), new Color(0.6f,0.1f,0.4f) });
            GenerateIconSet("Passive", new string[] { "toughness", "strength", "agility", "intellect", "vitality", "spirit", "critical", "dodge", "block", "regeneration" }, new Color[] { new Color(0.5f,0.5f,0.5f), new Color(0.9f,0.2f,0.2f), new Color(0.2f,0.9f,0.2f), new Color(0.2f,0.2f,0.9f), new Color(0.9f,0.5f,0.2f), new Color(0.7f,0.2f,0.9f), new Color(0.9f,0.9f,0.1f), new Color(0.3f,0.7f,0.3f), new Color(0.6f,0.4f,0.2f), new Color(0.2f,0.8f,0.4f) });
            GenerateIconSet("Weapons", new string[] { "sword_iron", "sword_steel", "sword_mithril", "sword_titanium", "shield_iron", "shield_steel", "shield_mithril", "shield_titanium", "dagger", "bow", "staff", "wand" }, new Color[] { new Color(0.6f,0.4f,0.2f), new Color(0.7f,0.7f,0.7f), new Color(0.3f,0.7f,0.9f), new Color(0.9f,0.8f,0.3f), new Color(0.5f,0.3f,0.1f), new Color(0.6f,0.6f,0.6f), new Color(0.2f,0.6f,0.8f), new Color(0.8f,0.7f,0.2f), new Color(0.4f,0.4f,0.4f), new Color(0.3f,0.5f,0.2f), new Color(0.6f,0.3f,0.8f), new Color(0.8f,0.3f,0.9f) });
            GenerateIconSet("Armor", new string[] { "linen", "silk", "magic", "epic", "plate", "leather", "mail" }, new Color[] { new Color(0.9f,0.9f,0.8f), new Color(0.9f,0.7f,0.9f), new Color(0.5f,0.3f,0.8f), new Color(0.9f,0.5f,0.1f), new Color(0.7f,0.7f,0.7f), new Color(0.6f,0.4f,0.2f), new Color(0.5f,0.5f,0.6f) });
            GenerateIconSet("Consumable", new string[] { "health_small", "health_medium", "health_large", "mana_small", "mana_medium", "mana_large", "strength", "agility", "intellect", "elixir_immortality", "elixir_absorption" }, new Color[] { new Color(0.9f,0.2f,0.2f), new Color(0.9f,0.3f,0.3f), new Color(0.9f,0.4f,0.4f), new Color(0.2f,0.2f,0.9f), new Color(0.3f,0.3f,0.9f), new Color(0.4f,0.4f,0.9f), new Color(0.9f,0.6f,0.1f), new Color(0.2f,0.8f,0.2f), new Color(0.4f,0.4f,0.9f), new Color(0.9f,0.8f,0.2f), new Color(0.6f,0.2f,0.8f) });
            GenerateIconSet("Accessories", new string[] { "ring_strength", "ring_agility", "ring_intellect", "ring_power", "amulet_vitality", "amulet_spirit", "amulet_immortality" }, new Color[] { new Color(0.9f,0.3f,0.1f), new Color(0.2f,0.8f,0.2f), new Color(0.2f,0.2f,0.9f), new Color(0.9f,0.9f,0.2f), new Color(0.9f,0.5f,0.2f), new Color(0.7f,0.2f,0.9f), new Color(0.9f,0.8f,0.2f) });
            GenerateIconSet("Engineering", new string[] { "defibrillator", "explosive", "invisibility", "rocket_pack", "turret" }, new Color[] { new Color(0.2f,0.8f,0.2f), new Color(0.9f,0.3f,0.1f), new Color(0.5f,0.5f,0.9f), new Color(0.9f,0.6f,0.1f), new Color(0.6f,0.6f,0.6f) });
            GenerateIconSet("Misc", new string[] { "quest_marker", "quest_complete", "gold", "silver", "copper", "reputation", "experience", "health", "mana" }, new Color[] { new Color(0.9f,0.9f,0.1f), new Color(0.2f,0.9f,0.2f), new Color(0.9f,0.8f,0.1f), new Color(0.7f,0.7f,0.7f), new Color(0.6f,0.4f,0.2f), new Color(0.5f,0.3f,0.8f), new Color(0.2f,0.6f,0.9f), new Color(0.9f,0.2f,0.2f), new Color(0.2f,0.2f,0.9f) });
            GenerateIconSet("Classes", new string[] { "warrior", "priest", "mage", "rogue", "hunter", "warlock" }, new Color[] { new Color(0.8f,0.3f,0.1f), new Color(0.9f,0.9f,0.3f), new Color(0.3f,0.3f,0.9f), new Color(0.5f,0.5f,0.5f), new Color(0.2f,0.7f,0.2f), new Color(0.5f,0.2f,0.7f) });
            GenerateIconSet("Spirits", new string[] { "guardian", "healer", "destroyer", "support" }, new Color[] { new Color(0.4f,0.4f,0.8f), new Color(0.2f,0.8f,0.4f), new Color(0.9f,0.3f,0.1f), new Color(0.8f,0.6f,0.2f) });
            GenerateIconSet("Professions", new string[] { "mining", "herbalism", "skinning", "blacksmithing", "alchemy", "tailoring", "engineering", "jewelcrafting" }, new Color[] { new Color(0.6f,0.4f,0.2f), new Color(0.2f,0.8f,0.2f), new Color(0.7f,0.5f,0.3f), new Color(0.7f,0.7f,0.7f), new Color(0.2f,0.6f,0.9f), new Color(0.9f,0.7f,0.9f), new Color(0.5f,0.5f,0.9f), new Color(0.9f,0.8f,0.2f) });
            GenerateIconSet("Quality", new string[] { "common", "rare", "epic", "legendary" }, new Color[] { new Color(1f,1f,1f), new Color(0.1f,1f,0.1f), new Color(0.6f,0.2f,0.9f), new Color(0.9f,0.5f,0.1f) });
            AssetDatabase.Refresh();
            Debug.Log("Placeholder icons generated successfully!");
        }

        private static void GenerateIconSet(string folder, string[] names, Color[] colors)
        {
            string basePath = "Assets/Icons/" + folder;
            int size = 64;
            for (int i = 0; i < names.Length; i++)
            {
                string fileName = names[i] + ".png";
                string fullPath = Path.Combine(basePath, fileName);
                if (File.Exists(fullPath)) continue;
                Color color = (i < colors.Length) ? colors[i] : Color.gray;
                Texture2D tex = new Texture2D(size, size, TextureFormat.RGBA32, false);
                Color[] pixels = new Color[size * size];
                for (int y = 0; y < size; y++)
                {
                    for (int x = 0; x < size; x++)
                    {
                        bool isBorder = (x < 2 || x >= size - 2 || y < 2 || y >= size - 2);
                        pixels[y * size + x] = isBorder ? Color.black : color;
                    }
                }
                tex.SetPixels(pixels);
                tex.Apply();
                byte[] pngData = tex.EncodeToPNG();
                File.WriteAllBytes(fullPath, pngData);
                string metaPath = fullPath + ".meta";
                if (!File.Exists(metaPath))
                {
                    string guid = System.Guid.NewGuid().ToString("N");
                    string metaContent = "fileFormatVersion: 2\nguid: " + guid + "\nTextureImporter:\n  textureType: 2\n  spriteMode: 1\n  filterMode: 1\n";
                    File.WriteAllText(metaPath, metaContent);
                }
                DestroyImmediate(tex);
            }
        }
    }
}
