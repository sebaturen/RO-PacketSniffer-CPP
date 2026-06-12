#include "packets/receive/UpgradeItemObtain.h"

namespace UpgradeItemObtainAPI
{
    constexpr const char* UPGRADE_ITEM_API_ENDPOINT = "items/obtain/upgrade";
}

void UpgradeItemObtain::deserialize_internal(const ReceivePacketTable pk_header)
{
    character_name = std::string(reinterpret_cast<const char*>(pkt_data.data()));
    item_id = pkt_data[24] | pkt_data[25] << 8 | pkt_data[26] << 16 | pkt_data[27] << 24;
    level = pkt_data[28] | pkt_data[29] << 8;
    unknown_val = pkt_data[30] | pkt_data[31] << 16;
}

void UpgradeItemObtain::report_upgrade_item()
{
    nlohmann::json data = {
        {"character_name", string_to_hex(character_name)},
        {"item_id", item_id},
        {"level", level},
        {"unknown_val", unknown_val}
    };
    
    send_request(UpgradeItemObtainAPI::UPGRADE_ITEM_API_ENDPOINT, data);
}
