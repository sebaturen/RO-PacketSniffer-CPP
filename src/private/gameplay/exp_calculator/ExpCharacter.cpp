#include "gameplay/exp_calculator/ExpCharacter.h"

#include <shared_mutex>

// ------------ Exp Hour ------------

double ExpHour::get_exp_hour()
{
    clean_old_events();

    if (!exp_events || exp_events->size() < 2)
        return -1;

    std::deque<std::pair<std::chrono::steady_clock::time_point, int64_t>> events_copy;
    events_copy.assign(exp_events->begin(), exp_events->end());
    
    auto t_start = events_copy.front().first;
    auto exp_start = events_copy.front().second;
    
    auto t_end = events_copy.back().first;
    auto exp_end = events_copy.back().second;

    auto minuts = std::chrono::duration<double>(t_end - t_start).count() / 60.0;

    if (minuts <= 0.0) return 0.0;

    return (exp_end - exp_start) / minuts * 60.0;
        
}

void ExpHour::add_exp(int64_t exp)
{
    current_exp += exp;
        
    auto now = std::chrono::steady_clock::now();
    
    exp_events->emplace_back(now, current_exp);
    clean_old_events();
}

void ExpHour::clean_old_events() const
{
    auto now = std::chrono::steady_clock::now();
    auto limit = std::chrono::minutes(20);

    while (!exp_events->empty() && now - exp_events->front().first > limit) {
        std::unique_lock lock(exp_history_mutex_);
        exp_events->pop_front();
    }
}

// ------------ Exp Character ------------

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
        // JOB
    case StatType::CURRENT_JOB_EXP:
        job_exp.current_exp = stat_info->get_value();
        break;
    case StatType::TOTAL_JOB_EXP:
        job_exp.total_required_exp = stat_info->get_value();
        break;
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
        std::unique_lock lock(base_exp.exp_history_mutex_);
        base_exp.exp_events->clear();
    }
    if (level_up_info->get_type() == UnitLevelUpType::JOB)
    {
        job_exp.level++;
        job_exp.current_exp = 0;
        std::unique_lock lock(job_exp.exp_history_mutex_);
        job_exp.exp_events->clear();
    }
}
