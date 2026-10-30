using UnityEngine.UIElements;
using UnityEngine;
using DarkMatter.Network;
using System;
using System.Collections.Generic;
using System.Linq;

namespace DM.Client
{
    public class CraftWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        [SerializeField] private UIDocument uiDocument;

        private ListView professionList;
        private ListView recipeList;
        private ListView materialList;
        private Label selectedProfessionLabel;
        private Label selectedRecipeLabel;
        private Label craftTimeLabel;
        private Label skillLevelLabel;
        private Button craftButton;
        private Button learnProfessionButton;

        private int selectedProfessionId = 0;
        private int selectedRecipeId = 0;
        private uint playerGuid = 0;
        private uint playerLevel = 1;

        private Dictionary<int, int> playerSkills = new Dictionary<int, int>();
        private Dictionary<int, int> playerProfessions = new Dictionary<int, int>();
        private HashSet<int> knownRecipes = new HashSet<int>();

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        private void OnEnable()
        {
            if (uiDocument == null) uiDocument = GetComponent<UIDocument>();
            var root = uiDocument.rootVisualElement;

            professionList = root.Q<ListView>("profession-list");
            recipeList = root.Q<ListView>("recipe-list");
            materialList = root.Q<ListView>("material-list");
            selectedProfessionLabel = root.Q<Label>("selected-profession");
            selectedRecipeLabel = root.Q<Label>("selected-recipe");
            craftTimeLabel = root.Q<Label>("craft-time");
            skillLevelLabel = root.Q<Label>("skill-level");
            craftButton = root.Q<Button>("craft-button");
            learnProfessionButton = root.Q<Button>("learn-profession-button");

            if (craftButton != null) craftButton.clicked += OnCraftClicked;
            if (learnProfessionButton != null) learnProfessionButton.clicked += OnLearnProfessionClicked;

            RefreshProfessions();
        }

        private void OnDisable()
        {
            if (craftButton != null) craftButton.clicked -= OnCraftClicked;
            if (learnProfessionButton != null) learnProfessionButton.clicked -= OnLearnProfessionClicked;
        }

        public void SetPlayerInfo(uint guid, uint level)
        {
            playerGuid = guid;
            playerLevel = level;
            RefreshProfessions();
        }

        public void SetPlayerSkill(int professionId, int skillLevel)
        {
            playerSkills[professionId] = skillLevel;
            RefreshRecipes();
        }

        public void SetPlayerProfession(int professionId, int skillLevel)
        {
            playerProfessions[professionId] = skillLevel;
            playerSkills[professionId] = skillLevel;
            RefreshProfessions();
        }

        public void LearnRecipe(int recipeId)
        {
            knownRecipes.Add(recipeId);
            RefreshRecipes();
        }

        public void SetKnownRecipes(IEnumerable<int> recipes)
        {
            knownRecipes.Clear();
            foreach (var r in recipes) knownRecipes.Add(r);
            RefreshRecipes();
        }
        private void RefreshProfessions()
        {
            if (professionList == null) return;

            var profItems = new List<string>();
            foreach (var prof in CraftingData.Professions)
            {
                bool hasProf = playerProfessions.ContainsKey(prof.Id);
                string skillStr = hasProf ? $" (Навык: {playerSkills.GetValueOrDefault(prof.Id, 0)}/{prof.MaxSkill})" : " (Не изучено)";
                profItems.Add($"{prof.NameRu}{skillStr}");
            }

            professionList.itemsSource = profItems;
            professionList.makeItem = () => new Label();
            professionList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = profItems[index];
            };
            professionList.selectionChanged += OnProfessionSelected;
        }

        private void OnProfessionSelected(IEnumerable<object> selectedItems)
        {
            var index = professionList.selectedIndex;
            if (index < 0 || index >= CraftingData.Professions.Count) return;

            selectedProfessionId = CraftingData.Professions[index].Id;
            if (selectedProfessionLabel != null)
                selectedProfessionLabel.text = CraftingData.Professions[index].NameRu;

            bool hasProf = playerProfessions.ContainsKey(selectedProfessionId);
            if (learnProfessionButton != null)
                learnProfessionButton.style.display = hasProf ? DisplayStyle.None : DisplayStyle.Flex;

            RefreshRecipes();
        }

        private void RefreshRecipes()
        {
            if (recipeList == null) return;

            var profRecipes = CraftingData.Recipes.Where(r => r.ProfessionId == selectedProfessionId).ToList();
            var recipeItems = new List<string>();

            foreach (var recipe in profRecipes)
            {
                bool known = knownRecipes.Contains(recipe.Id);
                bool canCraft = CanCraft(recipe);
                string status = known ? (canCraft ? " OK" : " WARN") : " LOCKED";
                recipeItems.Add($"{recipe.NameRu}{status}");
            }

            recipeList.itemsSource = recipeItems;
            recipeList.makeItem = () => new Label();
            recipeList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = recipeItems[index];
            };
            recipeList.selectionChanged += OnRecipeSelected;
        }

        private void OnRecipeSelected(IEnumerable<object> selectedItems)
        {
            var index = recipeList.selectedIndex;
            var profRecipes = CraftingData.Recipes.Where(r => r.ProfessionId == selectedProfessionId).ToList();
            if (index < 0 || index >= profRecipes.Count) return;

            selectedRecipeId = profRecipes[index].Id;
            var recipe = profRecipes[index];

            if (selectedRecipeLabel != null)
                selectedRecipeLabel.text = recipe.NameRu;

            if (craftTimeLabel != null)
                craftTimeLabel.text = $"Craft time: {recipe.CraftTimeMs / 1000f:F1}s";

            if (skillLevelLabel != null)
            {
                int skill = playerSkills.GetValueOrDefault(selectedProfessionId, 0);
                skillLevelLabel.text = $"Skill: {skill}/{recipe.RequiredSkill} (required)";
            }

            RefreshMaterials(recipe);

            if (craftButton != null)
            {
                bool canCraft = CanCraft(recipe);
                craftButton.SetEnabled(canCraft);
            }
        }

        private void RefreshMaterials(RecipeData recipe)
        {
            if (materialList == null) return;

            var matItems = new List<string>();
            foreach (var mat in recipe.Materials)
            {
                int have = GetItemCount(mat.ItemId);
                string status = have >= mat.Count ? "OK" : "MISSING";
                matItems.Add($"Item {mat.ItemId}: {have}/{mat.Count} {status}");
            }

            materialList.itemsSource = matItems;
            materialList.makeItem = () => new Label();
            materialList.bindItem = (element, index) =>
            {
                var label = (Label)element;
                label.text = matItems[index];
            };
        }

        private bool CanCraft(RecipeData recipe)
        {
            if (!knownRecipes.Contains(recipe.Id)) return false;
            if (!playerProfessions.ContainsKey(recipe.ProfessionId)) return false;
            int skill = playerSkills.GetValueOrDefault(recipe.ProfessionId, 0);
            if (skill < recipe.RequiredSkill) return false;
            foreach (var mat in recipe.Materials)
            {
                if (GetItemCount(mat.ItemId) < mat.Count) return false;
            }
            return true;
        }

        private int GetItemCount(int itemId)
        {
            return 0;
        }

        private void OnCraftClicked()
        {
            if (selectedRecipeId == 0) return;
            Craft(playerGuid, (uint)selectedRecipeId);
        }

        private void OnLearnProfessionClicked()
        {
            if (selectedProfessionId == 0) return;
            var prof = CraftingData.Professions.FirstOrDefault(p => p.Id == selectedProfessionId);
            if (prof == null) return;
            if (playerLevel < prof.MinLevel)
            {
                Debug.Log($"Requires level {prof.MinLevel} to learn {prof.NameRu}");
                return;
            }
            LearnProfession(playerGuid, (uint)selectedProfessionId);
        }

        public void Craft(uint playerGuid, uint recipeId)
        {
            Debug.Log($"Crafting recipe {recipeId} for player {playerGuid}");
            if (networkClient != null)
            {
                // TODO: send CMSG_CRAFT
            }
        }

        public void LearnRecipe(uint playerGuid, uint recipeId)
        {
            Debug.Log($"Learning recipe {recipeId} for player {playerGuid}");
            if (networkClient != null)
            {
                // TODO: send CMSG_LEARN_RECIPE
            }
        }

        public void LearnProfession(uint playerGuid, uint professionId)
        {
            Debug.Log($"Learning profession {professionId} for player {playerGuid}");
            if (networkClient != null)
            {
                // TODO: send CMSG_LEARN_PROFESSION
            }
        }
    }
}
