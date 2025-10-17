#include "packets/receive/SpecialItemObtain.h"

namespace ActorInfoAPI
{
    constexpr const char* SPECIAL_ITEM_OBTAIN_API_ENDPOINT = "items/obtain/special";
}

void SpecialItemObtain::deserialize_internal(const ReceivePacketTable pk_header)
{
    type = pkt_data[0];
    received_item_id = pkt_data[1] | (pkt_data[2] << 8) | (pkt_data[3] << 16) | (pkt_data[4] << 24);
    character_name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 6));
    
    if (type == 0)
    {
        size_t pkSize = pkt_data.size() - 4;
        from_item_id = pkt_data[pkSize] | (pkt_data[pkSize + 1] << 8) | (pkt_data[pkSize + 2] << 16) | (pkt_data[pkSize + 3] << 24);        
    }
    else if (type == 1)
    {
        size_t monsterNameStart = 6 + pkt_data[5] + 1;
        from_monster_code = std::string(reinterpret_cast<const char*>(pkt_data.data() + monsterNameStart));
        std::erase(from_monster_code, '\x1C');
    }
    
    report_special_item_obtain();
}

void SpecialItemObtain::report_special_item_obtain()
{
    nlohmann::json data = {
        {"type", type},
        {"received_item_id", received_item_id},
        {"from_item_id", from_item_id},
        {"from_monster_code", string_to_hex(from_monster_code)},
        {"character_name", string_to_hex(character_name)}
    };
    
    send_request(ActorInfoAPI::SPECIAL_ITEM_OBTAIN_API_ENDPOINT, data);
}
