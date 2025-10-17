#include "packets/receive/VenderFound.h"

#include <iostream>

void VenderFound::deserialize_internal(const ReceivePacketTable pk_header)
{
    // account id can be a clone id for offline shop
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    shop_name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 4));

    //std::cout << "Shop found: ["<< account_id <<"] " << shop_name << std::endl;
}
