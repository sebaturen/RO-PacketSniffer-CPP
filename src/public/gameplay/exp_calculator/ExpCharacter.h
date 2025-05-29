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
    
    ExpCharacter(const ReceivedCharacter& received_character);

    uint32_t get_character_id() const { return character_id; }
    void add_exp(const Exp& exp);
    void add_stat_info(const StatInfo& stat_info);
    void level_up(const UnitLevelUp& level_up_info);
    
private:

    // Base character info
    uint32_t character_id = 0;
    uint64_t base_level = 0;
    uint64_t job_level = 0;
    std::string name;

    // Exp info
    uint64_t total_base_exp = 0;
    uint64_t total_job_exp = 0;
    uint64_t current_base_exp = 0;
    uint64_t current_job_exp = 0;

    std::time_t last_base_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::time_t last_job_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
};
