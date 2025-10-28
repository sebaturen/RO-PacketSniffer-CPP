#include "packets/receive/VenderFound.h"

#include <iostream>

#include "gameplay/character/Character.h"

namespace VenderFoundAPI
{    
    constexpr const char* SHOP_API_ENDPOINT = "shop";
}

void VenderFound::deserialize_internal(const ReceivePacketTable pk_header)
{
    // account id can be a clone id for offline shop
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    shop_name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 4));
    if (Character::get_map(pid, shop_map))
    {
        report_shop();
    }
}

void VenderFound::report_shop()
{
    nlohmann::json data = {
        {"account_id", account_id},
        {"shop_name", string_to_hex(shop_name)},
        {"shop_map", string_to_hex(shop_map)}
    };

    send_request(VenderFoundAPI::SHOP_API_ENDPOINT, data);
}