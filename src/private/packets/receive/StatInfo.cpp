#include "packets/receive/StatInfo.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"
#include "gameplay/exp_calculator/ExpCharacter.h"

void StatInfo::deserialize_internal(const PacketInfo pk_header)
{
    type = static_cast<StatType>(pkt_data[0] | (pkt_data[1] << 8));
    val =
        static_cast<int64_t>(pkt_data[2]) |
        (static_cast<int64_t>(pkt_data[3]) << 8) |
        (static_cast<int64_t>(pkt_data[4]) << 16) |
        (static_cast<int64_t>(pkt_data[5]) << 24) |
        (static_cast<int64_t>(pkt_data[6]) << 32) |
        (static_cast<int64_t>(pkt_data[7]) << 40) |
        (static_cast<int64_t>(pkt_data[8]) << 48) |
        (static_cast<int64_t>(pkt_data[9]) << 56);
    
    if (ExpCharacter* c = ExpCalculator::get_char(port))
    {
        c->add_stat_info(*this);
    }
}
