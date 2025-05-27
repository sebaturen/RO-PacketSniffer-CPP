#include "gameplay/exp_calculator/ExpCalculator.h"

#include <ranges>

std::pmr::unordered_map<uint16_t, ExpCharacter> ExpCalculator::active_characters;
std::pmr::unordered_map<uint32_t, ReceivedCharacter> ExpCalculator::all_character;

void ExpCalculator::add_characters(const ReceivedCharacters& in_characters)
{
    for (const ReceivedCharacter& character : in_characters.get_characters())
    {
        all_character.emplace(character.get_character_id(), character);
    }
}

void ExpCalculator::set_used_character_id(uint16_t port, uint32_t character_id)
{
    for (auto& character : active_characters | std::views::values)
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

ExpCharacter* ExpCalculator::get_char(const uint16_t port)
{
    const auto it = active_characters.find(port);
    if (it != active_characters.end())
    {
        return &it->second;
    }
    
    return nullptr;
}