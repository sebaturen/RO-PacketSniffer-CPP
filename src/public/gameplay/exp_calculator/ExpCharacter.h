#pragma once

#include <chrono>
#include <ctime>

#include "packets/receive/ActorInfo.h"
#include "packets/receive/Exp.h"
#include "packets/receive/ReceivedCharacters.h"
#include "packets/receive/StatInfo.h"
#include "packets/receive/UnitLevelUp.h"

struct ExpHour
{
    float get_exp_hour();
    void add_exp(int64_t exp);
    void clean_old_events();
    
    uint64_t level = 0;
    uint64_t total_required_exp = 0;
    uint64_t current_exp = 0;
    std::deque<std::pair<std::chrono::steady_clock::time_point, int64_t>> exp_events; // <time, exp>

};

class ExpCharacter
{
public:
    
    ExpCharacter(const ReceivedCharacter* received_character);

    void add_exp(const Exp* exp);
    void add_stat_info(const StatInfo* stat_info);
    void level_up(const UnitLevelUp* level_up_info);
    
    uint32_t get_character_id() const { return character_id; }
    std::string get_name() const { return name; }

    const ExpHour& get_base_exp() const { return base_exp; }
    const ExpHour& get_job_exp() const { return job_exp; }
    
private:

    // Base character info
    uint32_t character_id = 0;
    std::string name;

    ExpHour base_exp;
    ExpHour job_exp;
};
