using UnityEngine;

[CreateAssetMenu(fileName = "Quest101Data", menu = "Quests/Quest101")]
public class Quest101Data : Quest50_60Data
{
    public Quest101Data()
    {
        questId = 101;
        titleRu = "Путь на Вершину";
        titleEn = "Path to the Summit";
        descriptionRu = "Покорите 10 горных пиков";
        descriptionEn = "Climb 10 mountain peaks";
        levelMin = 50;
        levelMax = 55;
        questType = "kill_monsters";
        questCategory = "exploration";
        objectives = "defend_pass, slay_10_mountain_trolls, retrieve_ancient_artifact";
        rewardType = "gold";
        rewardValue = "200";
        timeLimit = 0;
        isRepeatable = false;
        factionId = 1;
    }
}