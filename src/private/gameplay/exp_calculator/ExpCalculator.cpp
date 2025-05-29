#include "gameplay/exp_calculator/ExpCalculator.h"

#include <iostream>
#include <ranges>

std::pmr::unordered_map<uint16_t, ExpCharacter> ExpCalculator::active_characters;
std::pmr::unordered_map<uint32_t, ReceivedCharacter> ExpCalculator::all_character;

void ExpCalculator::add_characters(const ReceivedCharacters& in_characters)
{
    for (const ReceivedCharacter& character : in_characters.get_characters())
    {
        // si llega uno existente actualizar no agregar... 
        all_character.emplace(character.get_character_id(), character);
    }
}

void ExpCalculator::set_used_character_id(uint16_t port, uint32_t character_id)
{
    for (const ExpCharacter& character : active_characters | std::views::values)
    {
        if (character.get_character_id() == character_id)
        {
            // If character already exists, just update the port
            active_characters.erase(port);
            active_characters.emplace(port, character);
            return;
        }
    }
    
    // Add new character if it doesn't exist
    for (auto& character : all_character | std::views::values)
    {
        if (character.get_character_id() == character_id)
        {
            active_characters.emplace(port, ExpCharacter(character));
            return;
        }
    }
}

void ExpCalculator::notify_possible_change_port(uint16_t new_port)
{
    const auto old_char = active_characters.find(new_port-1);
    const auto new_char = active_characters.find(new_port);
    if (old_char != active_characters.end() && new_char == active_characters.end())
    {
        const ExpCharacter& character_copy = old_char->second;
        active_characters.emplace(new_port, character_copy);
        active_characters.erase(new_port-1);
    }
}

ExpCharacter* ExpCalculator::get_char(const uint16_t port)
{
    const auto it = active_characters.find(port);
    if (it != active_characters.end())
    {
        return &it->second;
    }
    
    return nullptr;
}

void ExpCalculator::show_exp()
{
    while (true)
    {
        std::cout << "\033[2J\033[H" << std::flush; // temp
        std::cout << "Active Characters:" << "\n";
        for (const ExpCharacter& character : active_characters | std::views::values)
        {
            std::cout << " " << character.get_name() << " (" << character.get_base_level() << "/" << character.get_job_level() << "): ";
            std::cout << "Base Exp: " << character.get_current_base_exp() << " / " << character.get_total_base_exp() << ", "
                      << "Job Exp: " << character.get_current_job_exp() << " / " << character.get_total_job_exp() << " ";
            std::cout << "Last Base Level Up: " << character.get_last_base_level_up() << ", "
                      << "Last Job Level Up: " << character.get_last_job_level_up() << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
