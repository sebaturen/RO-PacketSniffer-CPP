#pragma once

#include <unordered_map>

#include "ExpCharacter.h"

class ExpCalculator
{
public:

    static void add_characters(const ReceivedCharacters& in_characters);
    static void set_used_character_id(uint16_t port, uint32_t character_id);
    static void notify_possible_change_port(uint16_t new_port);
    static ExpCharacter* get_char(uint16_t port);
    static void show_exp();

private:

    static std::pmr::unordered_map<uint16_t, ExpCharacter> active_characters; // <port , Exp Character>
    static std::pmr::unordered_map<uint32_t, ReceivedCharacter> all_character; // < Character Id , Received Character>
};

