#include "packets/receive/ShopSoldLong.h"

namespace ShopSoldLongAPI
{
    constexpr const char* SHOP_SOLD_LONG_API_ENDPOINT = "shop/sold_long";
}

void ShopSoldLong::deserialize_internal(const PacketInfo pk_header)
{
    count = pkt_data[2] | (pkt_data[3] << 8);
    char_id = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
    timestamp = pkt_data[8] | (pkt_data[9] << 8) | (pkt_data[10] << 16) | (pkt_data[11] << 24);
    amount = pkt_data[12] | (pkt_data[13] << 8) | (pkt_data[14] << 16) | (pkt_data[15] << 24);

    report_sold();
}

void ShopSoldLong::report_sold()
{
    nlohmann::json shop_sold = {
        {"count", count},
        {"char_id", char_id},
        {"timestamp", timestamp},
        {"amount", amount}
    };

    send_request(ShopSoldLongAPI::SHOP_SOLD_LONG_API_ENDPOINT, shop_sold);
}
