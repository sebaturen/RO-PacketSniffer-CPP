#include "gameplay/exp_calculator/ExpCharacter.h"

ExpCharacter::ExpCharacter(const ReceivedCharacter& received_character)
{    
    character_id = received_character.get_character_id();
    base_level = received_character.get_base_level();
    job_level = received_character.get_job_level();
    name = received_character.get_name();
}

void ExpCharacter::add_exp(const Exp& exp)
{
    if (exp.get_type() == ExpType::BASE)
    {
        current_base_exp += exp.get_exp();
    }
    else if (exp.get_type() == ExpType::JOB)
    {
        current_job_exp += exp.get_exp();
    }
}

void ExpCharacter::add_stat_info(const StatInfo& stat_info)
{
    if (stat_info.get_type() == StatType::TOTAL_JOB_EXP)
    {
        total_job_exp = stat_info.get_value();
    }

    switch (stat_info.get_type())
    {
    case StatType::CURRENT_BASE_EXP:
        current_base_exp = stat_info.get_value();
        break;
    case StatType::CURRENT_JOB_EXP:
        current_job_exp = stat_info.get_value();
        break;
    case StatType::TOTAL_BASE_EXP:
        total_base_exp = stat_info.get_value();
        break;
    case StatType::TOTAL_JOB_EXP:
        total_job_exp = stat_info.get_value();
        break;
    default: 
        break;
    }
}
