using UnityEngine;

[CreateAssetMenu(fileName = "Quest102Data", menuName = "Quests/Quest102")]
public class Quest102Data : Quest50_60Data
{
    public Quest102Data()
    {
        questId = 102;
        titleRu = "Забытыйхранилище";
        titleEn = "Forgotten Vault";
        descriptionRu = "Найдите древнийartifact в заброшенном храме";
        descriptionEn = "Find ancient artifact in abandoned temple";
        levelMin = 55;
        levelMax = 60;
        questType = "collect_items";
        questCategory = "dungeon";
        objectives = "collect_5_artifact_fragments, defeat_ancient_guardian, retrieve_lost_knowledge";
        rewardType = "spirit";
        rewardValue = "150";
        timeLimit = 0;
        isRepeatable = false;
        factionId = 2;
    }
}