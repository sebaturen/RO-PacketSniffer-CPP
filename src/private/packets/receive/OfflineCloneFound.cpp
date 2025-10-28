#include "packets/receive/OfflineCloneFound.h"

#include <iostream>

#include "gameplay/character/Character.h"

namespace OfflineCloneFoundAPI
{
    constexpr const char* OFFLINE_API_ENDPOINT = "character/offline";
}


void OfflineCloneFound::deserialize_internal(const ReceivePacketTable pk_header)
{
    clone_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    job_id = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
    name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 35));

    coord_x = pkt_data[8] | (pkt_data[9] << 8);
    coord_y = pkt_data[10] | (pkt_data[11] << 8);
    sex = pkt_data[12];
    
    if (Character::get_map(pid, map))
    {
        report_clone();
    }
}

void OfflineCloneFound::report_clone()
{
    nlohmann::json char_info = {
        {"job_id", job_id},
        {"sex", sex},
        {"name", string_to_hex(name) }
    };

    nlohmann::json data = {
        {"clone_id", clone_id},
        {"info", char_info},
        {"map", string_to_hex(map)},
        {"coord_x", coord_x},
        {"coord_y", coord_y}
    };

    send_request(OfflineCloneFoundAPI::OFFLINE_API_ENDPOINT, data);
}