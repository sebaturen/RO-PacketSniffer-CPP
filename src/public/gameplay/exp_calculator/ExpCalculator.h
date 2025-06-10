#pragma once

#include <unordered_map>

#include "ExpCharacter.h"

struct SyncAccount
{   
    uint32_t account_id = 0xFFFFFFFF;
    uint32_t pid = 0xFFFFFFFF;
    std::optional<ReceivedCharacters> characters;
    std::atomic<std::shared_ptr<ExpCharacter>> active_character{nullptr};
};

class ExpCalculator
{
public:

    static void add_characters(uint32_t pid, const ReceivedCharacters* in_characters);
    static void sync_account(uint32_t pid, uint32_t account_id);
    
    static void set_used_character_id(uint32_t pid, uint32_t character_id);
    static std::shared_ptr<ExpCharacter> get_char(uint32_t pid);
    static void show_exp();

private:

    static std::unique_ptr<SyncAccount> temp_account;
    static std::pmr::unordered_map<uint32_t, std::unique_ptr<SyncAccount>> all_accounts; // < PID , Accounts >

};

