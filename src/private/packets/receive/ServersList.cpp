#include "packets/receive/ServersList.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"

void ServersList::deserialize_internal(const PacketInfo pk_header)
{
    int32_t server_info_size = 165;
    int32_t header_data = 56;
    
    account_id = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);

    std::cout << "Server\tPlayers\tURL\n";
    for (size_t i = header_data; (i+server_info_size) < pkt_data.size(); i += server_info_size)
    {
        uint16_t port = pkt_data[i + 8] | (pkt_data[i + 9] << 8);
        std::string server_name = std::string(reinterpret_cast<const char*>(&pkt_data[i] + 10));
        uint32_t current_players = pkt_data[i + 30] | (pkt_data[i + 31] << 8) | (pkt_data[i + 32] << 16) | (pkt_data[i + 33] << 24);
        std::string url_connection = std::string(reinterpret_cast<const char*>(&pkt_data[i] + 36));

        std::cout << server_name << "\t" << current_players << "\t" << url_connection << "\n";
    }
    
    ExpCalculator::sync_account(pid, account_id);
}
