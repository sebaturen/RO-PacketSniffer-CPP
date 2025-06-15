#include "gameplay/exp_calculator/ExpCalculator.h"

#include <complex.h>
#include <iostream>
#include <shared_mutex>

#include "Sniffer.h"

static std::shared_mutex account_mutex_;
std::pmr::unordered_map<uint32_t, std::unique_ptr<SyncAccount>> ExpCalculator::all_accounts;

void ExpCalculator::clean_account(uint32_t pid, uint32_t account_id)
{    
    std::unique_lock lock(account_mutex_);
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        account->account_id = account_id;
        account->characters = std::nullopt; // Clear characters
        account->active_character.store(nullptr); // Clear active character
    }
}

void ExpCalculator::add_characters(uint32_t pid, const ReceivedCharacters* in_characters)
{
    std::unique_lock lock(account_mutex_);
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        if (!account->characters)
        {
            account->characters = *in_characters;
            return;
        }

        for (const ReceivedCharacter& character : in_characters->get_characters())
        {
            account->characters->add_or_update_character(character);            
        }
        return;
    }

    SyncAccount* new_account = new SyncAccount();
    new_account->pid = pid;
    new_account->characters = *in_characters;
    all_accounts.try_emplace(pid, new_account);
}

void ExpCalculator::sync_account(uint32_t pid, uint32_t account_id)
{
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        if (account->account_id != account_id)
        {
            std::unique_lock lock(account_mutex_);
            if (account->account_id == 0xFFFFFFFF)
            {
                account->account_id = account_id;
                return;
            }
            all_accounts.erase(pid);
        }
        return;
    }

    SyncAccount* new_account = new SyncAccount();
    new_account->pid = pid;
    new_account->account_id = account_id;
    
    all_accounts.try_emplace(pid, new_account);
}

void ExpCalculator::set_used_character_id(uint32_t pid, uint32_t character_id)
{
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        if (!account->characters)
        {
            return;
        }
        
        if (account->active_character.load() && account->active_character.load()->get_character_id() == character_id)
        {
            return; // Already set
        }

        for (const ReceivedCharacter& character : account->characters->get_characters())
        {
            if (character.get_character_id() == character_id)
            {
                std::unique_lock lock(account_mutex_);
                account->active_character.store(std::make_shared<ExpCharacter>(&character));
            }
        }
    }
    else
    {
        std::cout << "["<< pid << "] Failed to set character: " << character_id << '\n';    
    }
}

std::shared_ptr<ExpCharacter> ExpCalculator::get_char(uint32_t pid)
{
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        return it->second->active_character.load();
    }

    return nullptr;
}

std::shared_ptr<ExpCharacter> ExpCalculator::get_char(uint32_t pid, uint32_t account_id)
{
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        if (account->account_id == account_id)
        {
            return it->second->active_character.load();
        }
    }

    return nullptr;
}

void ExpCalculator::show_exp()
{
    std::locale my_locale("");
    std::cout.imbue(my_locale);
    while (true)
    {
        std::cout << "\033[2J\033[H" << std::flush; // temp
        std::cout << "Active Characters: ["<< all_accounts.size() <<"]" << "\n";
        for (const std::unique_ptr<SyncAccount>& account : all_accounts | std::views::values)
        {
            const std::shared_ptr<ExpCharacter> character = account->active_character.load();
            
            if (character == nullptr)
                continue;

            ExpHour& base_exp = character->get_base_exp();
            ExpHour& job_exp = character->get_job_exp();

            float base_exp_h = base_exp.get_exp_hour();
            float job_exp_h = job_exp.get_exp_hour();
            
            std::cout << " " << character->get_name() << " (" << base_exp.level << "/" << job_exp.level << "): ";
            std::cout << "Base Exp: " << base_exp.current_exp << " / " << base_exp.total_required_exp << ", "
                      << "Job Exp: " << job_exp.current_exp << " / " << job_exp.total_required_exp << " ";
            std::cout << "Exp/H Base: " << base_exp_h << ", "
                      << "Job: " << job_exp_h << "\n";

            float base_hours = base_exp_h > 0 ? (base_exp.total_required_exp - base_exp.current_exp) / base_exp_h : -1;
            float job_hours =job_exp_h > 0 ? (job_exp.total_required_exp - job_exp.current_exp) / job_exp_h : -1;
            
            if (base_hours >= 0)
            {
                int hours_int = static_cast<int>(base_hours);
                int minutes_int = static_cast<int>((base_hours - hours_int) * 60);
                std::cout << "Base Exp Time: " << hours_int << "h " << minutes_int << "m\n";
            }
            
            if (job_hours >= 0)
            {
                int hours_int = static_cast<int>(job_hours);
                int minutes_int = static_cast<int>((job_hours - hours_int) * 60);
                std::cout << "Job Exp Time: " << hours_int << "h " << minutes_int << "m\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
