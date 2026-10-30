using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Individual achievement entry in the list
/// </summary>
public class AchievementEntry : MonoBehaviour
{
    [Header("UI References")]
    public Text achievementName;
    public Text achievementProgress;
    public Button claimButton;
    public GameObject completedMark;
    public GameObject incompleteMark;

    private AchievementDef achievementDef;
    private AchievementProgress progress;

    public void Initialize(AchievementDef def, AchievementProgress prog)
    {
        achievementDef = def;
        progress = prog;

        // Set name
        if (achievementName != null)
            achievementName.text = def.name_ru;

        // Set progress
        if (achievementProgress != null)
        {
            if (progress.completed)
            {
                achievementProgress.text = "Завершено";
                claimButton.gameObject.SetActive(false);
                completedMark.SetActive(true);
                incompleteMark.SetActive(false);
            }
            else
            {
                achievementProgress.text = $"Прогресс: {progress.progress}/{def.condition_value}";
                claimButton.gameObject.SetActive(true);
                completedMark.SetActive(false);
                incompleteMark.SetActive(true);
            }
        }

        // Setup claim button
        if (claimButton != null)
        {
            var finalProgress = progress;
            var finalDef = achievementDef;
            claimButton.onClick.RemoveAllListeners();
            claimButton.onClick.AddListener(() => OnClaimClicked(finalProgress, finalDef));
        }
    }

    private void OnClaimClicked(AchievementProgress prog, AchievementDef def)
    {
        // Send packet to server to claim achievement
        // Player network component will handle CMSG_ACHIEVEMENT_CLAIM_REWARD
        Debug.Log($"Attempting to claim achievement: {def.name_ru}");
    }
}