#include "packets/receive/Exp.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"

void Exp::deserialize_internal(const PacketInfo pk_header)
{
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    exp =
        static_cast<int64_t>(pkt_data[4]) |
        (static_cast<int64_t>(pkt_data[5]) << 8) |
        (static_cast<int64_t>(pkt_data[6]) << 16) |
        (static_cast<int64_t>(pkt_data[7]) << 24) |
        (static_cast<int64_t>(pkt_data[8]) << 32) |
        (static_cast<int64_t>(pkt_data[9]) << 40) |
        (static_cast<int64_t>(pkt_data[10]) << 48) |
        (static_cast<int64_t>(pkt_data[11]) << 56);
    
    uint32_t exp_type = pkt_data[12] | (pkt_data[13] << 8) | (pkt_data[14] << 16) | (pkt_data[15] << 24);
    type = static_cast<ExpType>(exp_type);

    ExpCalculator::sync_account(pid, account_id);
    if (ExpCharacter* c = ExpCalculator::get_char(pid).get())
    {
        c->add_exp(this);
    }
}
