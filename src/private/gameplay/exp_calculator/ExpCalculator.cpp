#include "gameplay/exp_calculator/ExpCalculator.h"

#include <iostream>
#include <shared_mutex>

#include "Sniffer.h"

static std::shared_mutex account_mutex_;
std::pmr::unordered_map<uint32_t, std::unique_ptr<SyncAccount>> ExpCalculator::all_accounts;

void ExpCalculator::add_characters(uint32_t pid, const ReceivedCharacters* in_characters)
{
    auto it = all_accounts.find(pid);
    if (it != all_accounts.end())
    {
        SyncAccount* account = it->second.get();
        std::unique_lock lock(account_mutex_);
        account->characters = *in_characters;
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

void ExpCalculator::show_exp()
{
    while (true)
    {
        std::cout << "\033[2J\033[H" << std::flush; // temp
        std::cout << "Active Characters: ["<< all_accounts.size() <<"]" << "\n";
        for (const std::unique_ptr<SyncAccount>& account : all_accounts | std::views::values)
        {
            const std::shared_ptr<ExpCharacter> character = account->active_character.load();
            
            if (character == nullptr)
                continue;
                        
            std::cout << " " << character->get_name() << " (" << character->get_base_level() << "/" << character->get_job_level() << "): ";
            std::cout << "Base Exp: " << character->get_current_base_exp() << " / " << character->get_total_base_exp_required() << ", "
                      << "Job Exp: " << character->get_current_job_exp() << " / " << character->get_total_job_exp_required() << " ";
            std::cout << "Last Base Level Up: " << character->get_last_base_level_up() << ", "
                      << "Last Job Level Up: " << character->get_last_job_level_up() << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
