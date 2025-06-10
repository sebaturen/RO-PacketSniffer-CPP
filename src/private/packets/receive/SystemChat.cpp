#include "packets/receive/SystemChat.h"

namespace SystemChatAPI
{
    constexpr const char* SYSTEM_CHAT = "system_chat";
}


void SystemChat::deserialize_internal(const PacketInfo pk_header)
{
    std::string message(reinterpret_cast<const char*>(pkt_data.data()));
    
    nlohmann::json data = {
        {"message", message}
    };

    send_request(SystemChatAPI::SYSTEM_CHAT, data);
}
