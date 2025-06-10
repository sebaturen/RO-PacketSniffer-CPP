#include "gameplay/exp_calculator/ExpCharacter.h"

ExpCharacter::ExpCharacter(const ReceivedCharacter* received_character)
{    
    character_id = received_character->get_character_id();
    base_level = received_character->get_base_level();
    job_level = received_character->get_job_level();
    name = received_character->get_name();
    
    current_base_exp = received_character->get_current_base_exp();
    current_job_exp = received_character->get_current_job_exp();
}

void ExpCharacter::add_exp(const Exp* exp)
{
    if (exp->get_type() == ExpType::BASE)
    {
        current_base_exp += exp->get_exp();
    }
    else if (exp->get_type() == ExpType::JOB)
    {
        current_job_exp += exp->get_exp();
    }
}

void ExpCharacter::add_stat_info(const StatInfo* stat_info)
{
    if (stat_info->get_type() == StatType::TOTAL_JOB_EXP)
    {
        total_job_exp_required = stat_info->get_value();
    }

    switch (stat_info->get_type())
    {
    case StatType::CURRENT_BASE_EXP:
        current_base_exp = stat_info->get_value();
        break;
    case StatType::CURRENT_JOB_EXP:
        current_job_exp = stat_info->get_value();
        break;
    case StatType::TOTAL_BASE_EXP:
        total_base_exp_required = stat_info->get_value();
        break;
    case StatType::TOTAL_JOB_EXP:
        total_job_exp_required = stat_info->get_value();
        break;
    case StatType::CHANGE_BASE_LEVEL:
        base_level = stat_info->get_value();
        last_base_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        current_base_exp = 0;
        break;
    case StatType::CHANGE_JOB_LEVEL:
        job_level = stat_info->get_value();
        last_job_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        current_job_exp = 0;
        break;     
    default: 
        break;
    }
}

void ExpCharacter::level_up(const UnitLevelUp* level_up_info)
{
    if (level_up_info->get_type() == UnitLevelUpType::BASE)
    {
        base_level++;
        last_base_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        current_base_exp = 0;
    }
    if (level_up_info->get_type() == UnitLevelUpType::JOB)
    {
        job_level++;
        last_job_level_up = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        current_job_exp = 0;
    }
}
