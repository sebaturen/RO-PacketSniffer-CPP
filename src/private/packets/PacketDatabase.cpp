#include "../../public/packets/PacketDatabase.h"

PacketDatabase::PacketDatabase() {
    init();
}

PacketDatabase& PacketDatabase::instance()
{
    static PacketDatabase db;
    return db;
}

const packet_detail* PacketDatabase::get(const uint16_t& head)
{
    const auto& map = instance().packet_map;
    const auto it = map.find(head);
    if (it != map.end()) {
        return &it->second;
    }
    return nullptr;
}

void PacketDatabase::init()
{
    // https://github.com/OpenKore/openkore/blob/master/src/Network/Receive/ServerType0.pm
    packet_map[0x008e] = { .size= 0, .description= "Self chat"};
    packet_map[0x07fd] = { .size= 0, .description= "Special item obtained"};
    packet_map[0x007f] = { .size= 6, .description= "Recived sync" };
}
