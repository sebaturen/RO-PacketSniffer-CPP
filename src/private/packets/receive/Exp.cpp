#include "../../../public/packets/receive/Exp.h"

#include <iostream>

void Exp::deserialize_internal(const PacketInfo pk_header)
{
    actor_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);int64_t exp =
    exp = static_cast<int64_t>(pkt_data[4]) |
        (static_cast<int64_t>(pkt_data[5]) << 8) |
        (static_cast<int64_t>(pkt_data[6]) << 16) |
        (static_cast<int64_t>(pkt_data[7]) << 24) |
        (static_cast<int64_t>(pkt_data[8]) << 32) |
        (static_cast<int64_t>(pkt_data[9]) << 40) |
        (static_cast<int64_t>(pkt_data[10]) << 48) |
        (static_cast<int64_t>(pkt_data[11]) << 56);


    uint32_t exp_type = pkt_data[12] | (pkt_data[13] << 8) | (pkt_data[14] << 16) | (pkt_data[15] << 24);
    type = static_cast<ExpType>(exp_type);
    
    //debug_packet();
    std::cout << "[INFO] Exp packet. Actor ID: " << actor_id << " Exp: " << exp << " Type: " << static_cast<int>(type) << '\n';
}
