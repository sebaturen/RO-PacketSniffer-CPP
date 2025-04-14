#include "../../public/packets/PacketDatabase.h"

PacketDatabase::PacketDatabase() {
    init();
}

PacketDatabase& PacketDatabase::instance()
{
    static PacketDatabase db;
    return db;
}

const packet_detail* PacketDatabase::get(const std::string& id)
{
    const auto& map = instance().packet_map;
    const auto it = map.find(id);
    if (it != map.end()) {
        return &it->second;
    }
    return nullptr;
}

void PacketDatabase::init()
{
    // https://github.com/OpenKore/openkore/blob/master/src/Network/Receive/ServerType0.pm
    packet_map["0x0800"] = { .size= 20, .description= "IPv4"};
}
