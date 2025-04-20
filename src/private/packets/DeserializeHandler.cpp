#include "../../public/packets/DeserializeHandler.h"

#include <iomanip>
#include <iostream>

void DeserializeHandler::deserialize(const std::vector<uint8_t>* data)
{
    if (data == nullptr)
    {
        return;
    }

    const PacketInfo header = static_cast<PacketInfo>((*data)[0] | (*data)[1] << 8);
    const packet_detail* pkt_detail = PacketDatabase::get(header);

    uint8_t start_data = 2;
    if (pkt_detail->size == -1)
    {
        start_data = 4;
    }
    pkt_data = std::span(data->data() + start_data, data->size() - start_data);
    deserialize_internal(header);
}

void DeserializeHandler::debug_packet() const
{
    std::cout << "[";
    for (unsigned char i : pkt_data)
    {
        std::cout << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(i) << " ";
    }
    std::cout << "]" << std::dec << std::endl;
}
