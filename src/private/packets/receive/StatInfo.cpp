#include "packets/receive/StatInfo.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"
#include "gameplay/exp_calculator/ExpCharacter.h"

void StatInfo::deserialize_internal(const PacketInfo pk_header)
{
    uint32_t pk_type = pkt_data[0] | (pkt_data[1] << 8);
    
    val =
        static_cast<int64_t>(pkt_data[2]) |
        (static_cast<int64_t>(pkt_data[3]) << 8) |
        (static_cast<int64_t>(pkt_data[4]) << 16) |
        (static_cast<int64_t>(pkt_data[5]) << 24);
    
    if (pk_header == PacketInfo::STAT_INFO_0)
    {
        type = static_cast<StatType>(pk_type);
    }
    if (pk_header == PacketInfo::STAT_INFO_7)
    {
        uint32_t n_type = pk_type+(100*7);
        type = static_cast<StatType>(n_type);
        val |=
            (static_cast<int64_t>(pkt_data[6]) << 32) |
            (static_cast<int64_t>(pkt_data[7]) << 40) |
            (static_cast<int64_t>(pkt_data[8]) << 48) |
            (static_cast<int64_t>(pkt_data[9]) << 56);
    }

    if (StatType::TOTAL_BASE_EXP == type ||
        StatType::TOTAL_JOB_EXP == type)
    {
        if (ExpCharacter* c = ExpCalculator::get_char(pid).get())
        {
            c->add_stat_info(this);
        }
    }
    else
    {
        //if (pk_header == PacketInfo::STAT_INFO_0)
        //    std::cout << "[" << pid << "] StatInfo: " << static_cast<int>(type) << " = " << val << '\n';
    }
}
