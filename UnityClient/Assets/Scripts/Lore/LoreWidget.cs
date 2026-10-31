using UnityEngine;

public class LoreWidget : MonoBehaviour
{
    public Text contentText;
    public Button closeButton;
    
    void Start()
    {
        closeButton.onClick.AddListener(() => gameObject.SetActive(false));
    }
    
    public void ShowLore(int loreId)
    {
        // Load lore from SQL via server
        // For now, display static content
        contentText.text = GetLoreContent(loreId);
        gameObject.SetActive(true);
    }
    
    string GetLoreContent(int loreId)
    {
        // This would normally query the server SQL
        // For now, return sample content
        switch (loreId)
        {
            case 1:
                return "В начале был Хаос. Из Хаоса родились Три Силы: Свет, Тьма и Пустота.";
            case 2:
                return "Фейрологический Орден был основан первыми людьми, стремившимися к порядку и свету.";
            case 3:
                return "Темный Орден восстановил древние ритуалы Черной Магии для власти над смертными.";
            default:
                return "Лор не найден.";
        }
    }
}