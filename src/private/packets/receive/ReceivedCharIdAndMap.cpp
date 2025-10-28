#include "packets/receive/ReceivedCharIdAndMap.h"

#include "Sniffer.h"
#include "gameplay/character/Character.h"
#include "gameplay/exp_calculator/ExpCalculator.h"

void ReceivedCharIdAndMap::deserialize_internal(const ReceivePacketTable pk_header)
{
    character_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    map_name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 4));
    map_url = std::string(reinterpret_cast<const char*>(pkt_data.data() + 26));

    ExpCalculator::set_used_character_id(pid, character_id);
    Character::set_map(pid, map_name);

    /* Add new IP on sniff list on realtime --> need refactor,  `ro_latam_ip_list` is only call on start
    size_t colon_pos = map_url.find(':');
    if (colon_pos != std::string::npos)
    {
        std::string ip = map_url.substr(0, colon_pos);
        std::string port_str = map_url.substr(colon_pos + 1);

        // Check existence of the IP in the list
        if (std::ranges::find(SnifferSpace::ro_latam_ip_list, ip) == SnifferSpace::ro_latam_ip_list.end()) {
            SnifferSpace::ro_latam_ip_list.emplace_back(ip);
        }
    }*/
}
