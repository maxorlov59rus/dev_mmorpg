// Quest data structures for the entire level range
struct QuestData {
    int id;
    std::string title_ru;
    std::string title_en;
    std::string description_ru;
    std::string description_en;
    int level_min;
    int level_max;
    std::string quest_type;
    std::string quest_category;
    std::string objectives;
    std::string reward_type;
    std::string reward_value;
    int reward_amount;
    int time_limit;
    bool is_repeatable;
    int faction_id;
};

// NPC data structure
struct NpcData {
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

// Monster data structure
struct MonsterData {
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

// Resource data structure
struct ResourceData {
    int id;
    std::string name_ru;
    std::string name_en;
    std::string type;
    int stack_size;
    int respawn_time;
    int gathering_level_min;
    std::string description_ru;
    std::string description_en;
};