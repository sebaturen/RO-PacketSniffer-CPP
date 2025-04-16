#include "../../public/packets/PacketDatabase.h"
#include "../../public/packets/receive/SelfChat.h"

std::unordered_set<uint16_t> PacketDatabase::s_valid_ids;

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

const std::unordered_set<uint16_t>& PacketDatabase::get_all_ids()
{
    return s_valid_ids;    
}

void PacketDatabase::init()
{
    // https://github.com/OpenKore/openkore/blob/master/src/Network/Receive/ServerType0.pm
    packet_map[0x008e] = { .size = -1,   .handler = &g_selfChatHandler,   .description = "Self chat" };
    packet_map[0x007f] = { .size = 6,    .handler = nullptr,  .description = "Received sync" };
    packet_map[0x0187] = { .size = 6,    .handler = nullptr,  .description = "Sync request" };
    packet_map[0x0b1d] = { .size = 6,    .handler = nullptr, .description = "Ping" };
    packet_map[0x09cf] = { .size = -1,   .handler = nullptr, .description = "GameGuard request" };
    packet_map[0x01c8] = { .size = 15,   .handler = nullptr, .description = "Item used" };
    packet_map[0x00d8] = { .size = 6,    .handler = nullptr, .description = "Chat removed" };
    packet_map[0x01d7] = { .size = 11,   .handler = nullptr, .description = "Sprite change" }; // #11 or #15
    packet_map[0x019b] = { .size = 10,   .handler = nullptr, .description = "Unit level up" };
    packet_map[0x00c0] = { .size = 7,    .handler = nullptr, .description = "Emoticon" };
    packet_map[0x01aa] = { .size = 10,   .handler = nullptr, .description = "Pet emotion" };
    packet_map[0x0ae3] = { .size = -1,   .handler = nullptr, .description = "Received login token" };
    packet_map[0x0081] = { .size = 6,    .handler = nullptr, .description = "Errors" };
    packet_map[0x08b9] = { .size = 12,   .handler = nullptr, .description = "Login ping code request" };
    packet_map[0x006a] = { .size = 23,   .handler = nullptr, .description = "Login error" };
    packet_map[0x0ac7] = { .size = 156,  .handler = nullptr, .description = "Map changed" };
    packet_map[0x0446] = { .size = 14,   .handler = nullptr, .description = "Minimap indicator" };

    // Items
    packet_map[0x0add] = { .size = 24,   .handler = nullptr, .description = "Item info" };
    packet_map[0x00a1] = { .size = 6,    .handler = nullptr, .description = "Item disappeared" };
    packet_map[0x07fd] = { .size = -1,   .handler = nullptr, .description = "Special item obtained" };
    packet_map[0x0147] = { .size = 12,   .handler = nullptr, .description = "Item skill" };

    // Broadcast
    packet_map[0x01c3] = { .size = -1,   .handler = nullptr, .description = "Local broadcast" };
    packet_map[0x07fd] = { .size = -1,   .handler = nullptr, .description = "Special item obtained" };
    packet_map[0x008d] = { .size = -1,   .handler = nullptr, .description = "Public chat" };
    packet_map[0x00b4] = { .size = -1,   .handler = nullptr, .description = "NPC Talk" };
    packet_map[0x0291] = { .size = 6,    .handler = nullptr, .description = "Message string" };

    // Account
    packet_map[0x0ac4] = { .size = -1,   .handler = nullptr, .description = "Account server info" };

    // Actor
    packet_map[0x09ff]  = { .size = -1,  .handler = nullptr, .description = "Actor exists" };
    packet_map[0x09fd]  = { .size = -1,  .handler = nullptr, .description = "Actor move" };
    packet_map[0x0088]  = { .size = 10,  .handler = nullptr, .description = "Actor movement interrupted" };
    packet_map[0x0080]  = { .size = 7,   .handler = nullptr, .description = "Actor died or disappeared" };
    packet_map[0x009c]  = { .size = 9,   .handler = nullptr, .description = "Actor look at" };
    packet_map[0x0a30]  = { .size = 106, .handler = nullptr, .description = "Actor info" };
    packet_map[0x0adf]  = { .size = 58,  .handler = nullptr, .description = "Actor info 2" };
    packet_map[0x09fe]  = { .size = -1,  .handler = nullptr, .description = "Actor action" };
    packet_map[0x02e1]  = { .size = 33,  .handler = nullptr, .description = "Actor action 2" };
    packet_map[0x08c8]  = { .size = 34,  .handler = nullptr, .description = "Actor action 3" };
    packet_map[0x008a]  = { .size = 29,  .handler = nullptr, .description = "Actor action 4" };
    packet_map[0x0196]  = { .size = 9,   .handler = nullptr, .description = "Actor status active" };
    packet_map[0x0983]  = { .size = 29,  .handler = nullptr, .description = "Actor status active 2" };
    packet_map[0x043f]  = { .size = 25,  .handler = nullptr, .description = "Actor status active 3" };

    // Character
    packet_map[0x0087] = { .size = 12,   .handler = nullptr, .description = "Character Move" };
    packet_map[0x0229] = { .size = 15,   .handler = nullptr, .description = "Character status" };
    packet_map[0x0141] = { .size = 14,   .handler = nullptr, .description = "Character status info" };
    packet_map[0x082d] = { .size = -1,   .handler = nullptr, .description = "Character Info" };
    packet_map[0x09a0] = { .size = 6,    .handler = nullptr, .description = "Character received sync" };
    packet_map[0x0ac5] = { .size = 156,  .handler = nullptr, .description = "Character received ID and Map" };
    packet_map[0x00b0] = { .size = 8,    .handler = nullptr, .description = "Character stat info" };
    packet_map[0x020d] = { .size = -1,   .handler = nullptr, .description = "Character ban list" };
    packet_map[0x099d] = { .size = -1,   .handler = nullptr, .description = "Character received" };

    // Skill
    packet_map[0x0110] = { .size = 12,   .handler = nullptr, .description = "Skill use failed" }; // #12 or #14
    packet_map[0x09cb] = { .size = 17,   .handler = nullptr, .description = "Skill use no damage" };
    packet_map[0x07fb] = { .size = 25,   .handler = nullptr, .description = "Skill use" };
    
    // Monster
    packet_map[0x0a36] = { .size = 7,    .handler = nullptr, .description = "Monster HP info tiny" };

    // Inventory
    packet_map [0x0900] = { .size= -1,   .handler = nullptr, .description = "Inventory items stackable" };
    packet_map [0x00af] = { .size= 6,    .handler = nullptr, .description = "Inventory items stackable" };
    packet_map [0x0a37] = { .size= 69,   .handler = nullptr, .description = "Inventory item added" };

    // Unknown
    //packet_map[0x6b39] = { .size = 6,    .handler = nullptr, .description = "Unknown - InLogin char select" };
    
    for (const auto& [id, _] : packet_map)
        s_valid_ids.insert(id);
}
