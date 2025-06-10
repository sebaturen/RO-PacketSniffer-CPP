#pragma once

#include <chrono>
#include <ctime>

#include "packets/receive/ActorInfo.h"
#include "packets/receive/Exp.h"
#include "packets/receive/ReceivedCharacters.h"
#include "packets/receive/StatInfo.h"
#include "packets/receive/UnitLevelUp.h"

class ExpCharacter
{
public:
    
    ExpCharacter(const ReceivedCharacter* received_character);

    void add_exp(const Exp* exp);
    void add_stat_info(const StatInfo* stat_info);
    void level_up(const UnitLevelUp* level_up_info);
    
    uint32_t get_character_id() const { return character_id; }
    std::string get_name() const { return name; }
    uint64_t get_base_level() const { return base_level; }
    uint64_t get_job_level() const { return job_level; }
    uint64_t get_total_base_exp_required() const { return total_base_exp_required; }
    uint64_t get_total_job_exp_required() const { return total_job_exp_required; }
    uint64_t get_current_base_exp() const { return current_base_exp; }
    uint64_t get_current_job_exp() const { return current_job_exp; }
    std::time_t get_last_base_level_up() const { return last_base_level_up; }
    std::time_t get_last_job_level_up() const { return last_job_level_up; }
    
private:

    // Base character info
    uint32_t character_id = 0;
    std::string name;
    uint64_t base_level = 0;
    uint64_t job_level = 0;

    // Exp info
    uint64_t total_base_exp_required = 0;
    uint64_t total_job_exp_required = 0;
    uint64_t current_base_exp = 0;
    uint64_t current_job_exp = 0;

    // Timestamps for level up
    std::time_t last_base_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::time_t last_job_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
};
