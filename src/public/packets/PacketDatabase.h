#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "PackeTable.h"

class DeserializeHandler;

enum class PacketSizeType {
    FIXED,
    INDICATED_IN_PACKET,
    UNKNOWN,
    FIXED_MIN,
    HTTP,
};

struct packet_detail
{
    std::string desc;
    int16_t size;
    PacketSizeType type;
    DeserializeHandler* handler;
    bool alert = false;
};

class PacketDatabase
{
public:    
    static const packet_detail* get(const PacketInfo& head);
    
private:
    PacketDatabase();
    
    void init();
    
    static PacketDatabase& instance();
    
    std::unordered_map<PacketInfo, packet_detail> packet_map;
};