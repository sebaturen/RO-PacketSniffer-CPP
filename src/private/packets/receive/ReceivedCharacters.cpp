#include "packets/receive/ReceivedCharacters.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"

void ReceivedCharacters::deserialize_internal(const PacketInfo pk_header)
{    
    if (pkt_data.size() % 155 > 0)
    {
        std::cout << "[ERROR] ReceivedCharacters packet size is not a multiple of 155. Size: " << pkt_data.size() << std::endl;
        return;
    }

    for (size_t i = 0; i < pkt_data.size(); i += 155)
    {
        uint32_t character_id = pkt_data[i] | (pkt_data[i + 1] << 8) | (pkt_data[i + 2] << 16) | (pkt_data[i + 3] << 24);
        uint32_t job_level = pkt_data[i + 24] | (pkt_data[i + 25] << 8);
        uint32_t base_level = pkt_data[i + 72] | (pkt_data[i + 73] << 8);
        std::string name(reinterpret_cast<const char*>(pkt_data.data() + i + 88));

        uint64_t current_base_exp =
            static_cast<uint64_t>(pkt_data[i + 4]) |
            (static_cast<uint64_t>(pkt_data[i + 5]) << 8) |
            (static_cast<uint64_t>(pkt_data[i + 6]) << 16) |
            (static_cast<uint64_t>(pkt_data[i + 7]) << 24) |
            (static_cast<uint64_t>(pkt_data[i + 8]) << 32) |
            (static_cast<uint64_t>(pkt_data[i + 9]) << 40) |
            (static_cast<uint64_t>(pkt_data[i + 10]) << 48) |
            (static_cast<uint64_t>(pkt_data[i + 11]) << 56);

        uint64_t current_job_exp =
            static_cast<uint64_t>(pkt_data[i + 16]) |
            (static_cast<uint64_t>(pkt_data[i + 17]) << 8) |
            (static_cast<uint64_t>(pkt_data[i + 18]) << 16) |
            (static_cast<uint64_t>(pkt_data[i + 19]) << 24) |
            (static_cast<uint64_t>(pkt_data[i + 20]) << 32) |
            (static_cast<uint64_t>(pkt_data[i + 21]) << 40) |
            (static_cast<uint64_t>(pkt_data[i + 22]) << 48) |
            (static_cast<uint64_t>(pkt_data[i + 23]) << 56);
        
        characters.emplace_back(character_id, base_level, job_level, name, current_base_exp, current_job_exp);
    }

    ExpCalculator::add_characters(*this);
}
