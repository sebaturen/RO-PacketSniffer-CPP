#include "gameplay/exp_calculator/ExpCharacter.h"

float ExpHour::get_exp_hour()
{
    clean_old_events();

    if (exp_events.size() < 2)
        return -1;

    auto t_start = exp_events.front().first;
    auto exp_start = exp_events.front().second;
    
    auto t_end = exp_events.back().first;
    auto exp_end = exp_events.back().second;

    auto minuts = std::chrono::duration<double>(t_end - t_start).count() / 60.0;

    if (minuts <= 0.0) return 0.0;

    return (exp_end - exp_start) / minuts * 60.0;
        
}

void ExpHour::add_exp(int64_t exp)
{    
    current_exp += exp;
    
    auto now = std::chrono::steady_clock::now();
    exp_events.emplace_back(now, current_exp);
    clean_old_events();
}

void ExpHour::clean_old_events()
{    
    auto now = std::chrono::steady_clock::now();
    auto limit = std::chrono::minutes(10);

    while (!exp_events.empty() && now - exp_events.front().first > limit) {
        exp_events.pop_front();
    }
}

ExpCharacter::ExpCharacter(const ReceivedCharacter* received_character)
{    
    character_id = received_character->get_character_id();
    name = received_character->get_name();

    base_exp.level = received_character->get_base_level();
    job_exp.level = received_character->get_job_level();

    base_exp.current_exp = received_character->get_current_base_exp();
    job_exp.current_exp = received_character->get_current_job_exp();
}

void ExpCharacter::add_exp(const Exp* exp)
{
    if (exp->get_type() == ExpType::BASE)
    {
        base_exp.add_exp(exp->get_exp());
    }
    else if (exp->get_type() == ExpType::JOB)
    {
        job_exp.add_exp(exp->get_exp());
    }
}

void ExpCharacter::add_stat_info(const StatInfo* stat_info)
{    
    switch (stat_info->get_type())
    {
        // BASE
    case StatType::CURRENT_BASE_EXP:
        base_exp.current_exp = stat_info->get_value();
        break;
    case StatType::TOTAL_BASE_EXP:
        base_exp.total_required_exp = stat_info->get_value();
        break;
    /*case StatType::CHANGE_BASE_LEVEL:
        base_exp.level = stat_info->get_value();
        base_exp.current_exp = 0;
        base_exp.exp_events.clear();
        break;*/
        // JOB
    case StatType::CURRENT_JOB_EXP:
        job_exp.current_exp = stat_info->get_value();
        break;
    case StatType::TOTAL_JOB_EXP:
        job_exp.total_required_exp = stat_info->get_value();
        break;
    /*case StatType::CHANGE_JOB_LEVEL:
        if (stat_info->get_value() > 0)
        {
            job_exp.level = stat_info->get_value();
            job_exp.accumulated_exp = 0;            
        }
        break;*/
    default: 
        break;
    }
}

void ExpCharacter::level_up(const UnitLevelUp* level_up_info)
{
    if (level_up_info->get_type() == UnitLevelUpType::BASE)
    {
        base_exp.level++;
        base_exp.current_exp = 0;
        base_exp.exp_events.clear();
    }
    if (level_up_info->get_type() == UnitLevelUpType::JOB)
    {
        job_exp.level++;
        job_exp.current_exp = 0;
        job_exp.exp_events.clear();
    }
}
