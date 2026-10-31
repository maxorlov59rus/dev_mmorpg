// NPC data structure for level 50-60 content
struct Npc50_60Data {
    int id;
    std::string name_ru;
    std::string name_en;
    std::string title_ru;
    std::string title_en;
    int faction_id;
    int city_id;
    std::string npc_type;
    std::string rank;
    int gender;
    std::string race;
    bool is_quest_giver;
    bool is_trainer;
    bool is_special_interact;
    std::string description_ru;
    std::string description_en;
};

// Monster data structure for level 50-60 content
struct Monster50_60Data {
    int id;
    std::string name_ru;
    std::string name_en;
    int level;
    int faction_id;
    int hp;
    int mana;
    int exp_reward;
    int gold_reward;
    int attack_power;
    int defense;
    int quest_id;
};