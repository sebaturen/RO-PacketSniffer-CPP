#include "packets/receive/SystemChat.h"

namespace SystemChatAPI
{
    constexpr const char* SYSTEM_CHAT = "system/chat";
}


void SystemChat::deserialize_internal(const PacketInfo pk_header)
{
    nlohmann::json data;
    if (pkt_data[0] == 0x1c)
    {
        std::vector<std::string> params;
        size_t start_position = 1;
        size_t end_position = start_position;
        while (end_position <= pkt_data.size())
        {
            if (pkt_data[end_position] == 0x1c || pkt_data[end_position] == 0x1d)
            {
                std::string param = std::string(reinterpret_cast<const char*>(&pkt_data[start_position]), end_position - start_position);
                params.push_back(string_to_hex(param));
                start_position = end_position + 1;
            }
            
            if (pkt_data[end_position] == 0x1c)
            {
                break;
            }
            end_position++;
        }
        data = {
            {"params", params}
        };
    }
    else
    {        
        std::string message(reinterpret_cast<const char*>(pkt_data.data()));
        data = {
            {"message", string_to_hex(message)}
        };
    }
    

    send_request(SystemChatAPI::SYSTEM_CHAT, data);
}
