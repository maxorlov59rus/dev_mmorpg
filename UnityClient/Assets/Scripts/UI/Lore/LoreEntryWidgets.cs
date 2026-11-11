using UnityEngine;
using UnityEngine.UI;
using System;

/// <summary>
/// Entry widgets for the Lore browser and quest giver lists.
/// All entries expect Text/Button children found by name; missing references are tolerated.
/// </summary>
public class CityEntry : MonoBehaviour
{
    public CityInfo city;
    public LoreMgr loreManager;
    public event Action<CityInfo> onCitySelected;

    public Text titleText;

    public void Initialize(CityInfo data, LoreMgr mgr)
    {
        city = data;
        loreManager = mgr;

        if (titleText == null)
            titleText = GetComponentInChildren<Text>();

        if (titleText != null && data != null)
            titleText.text = data.name_ru;

        var button = GetComponentInChildren<Button>();
        if (button != null)
        {
            button.onClick.RemoveAllListeners();
            button.onClick.AddListener(() => onCitySelected?.Invoke(city));
        }
    }
}

public class NpcEntry : MonoBehaviour
{
    public NpcInfo npc;
    public LoreMgr loreManager;
    public event Action<NpcInfo> onNpcSelected;

    public Text titleText;
    public Text subtitleText;

    public void Initialize(NpcInfo data, LoreMgr mgr)
    {
        npc = data;
        loreManager = mgr;

        if (titleText == null)
            titleText = GetComponentInChildren<Text>();

        if (titleText != null && data != null)
            titleText.text = string.IsNullOrEmpty(data.name_ru) ? data.name : data.name_ru;

        if (subtitleText != null && data != null)
            subtitleText.text = string.IsNullOrEmpty(data.title_ru) ? data.title : data.title_ru;

        var button = GetComponentInChildren<Button>();
        if (button != null)
        {
            button.onClick.RemoveAllListeners();
            button.onClick.AddListener(() => onNpcSelected?.Invoke(npc));
        }
    }
}

public class QuestEntry : MonoBehaviour
{
    public QuestInfo quest;
    public LoreMgr loreManager;
    public event Action<QuestInfo> onQuestSelected;
    public event Action<QuestInfo> onQuestAccept;

    public Text titleText;
    public Text levelText;

    public void Initialize(QuestInfo data, LoreMgr mgr)
    {
        quest = data;
        loreManager = mgr;

        if (titleText == null)
            titleText = GetComponentInChildren<Text>();

        if (titleText != null && data != null)
            titleText.text = data.title_ru;

        if (levelText != null && data != null)
            levelText.text = $"{data.level_min}-{data.level_max}";

        var button = GetComponentInChildren<Button>();
        if (button != null)
        {
            button.onClick.RemoveAllListeners();
            button.onClick.AddListener(() =>
            {
                onQuestSelected?.Invoke(quest);
                onQuestAccept?.Invoke(quest);
            });
        }
    }
}
