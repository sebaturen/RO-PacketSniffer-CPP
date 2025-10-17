#pragma once

#include <string>
#include <unordered_map>

#include "DeserializeHandler.h"
#include "ReceivePacketTable.h"

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
    HandlerFactory handler;
    bool alert = false;
};

class PacketDatabase
{
public:    
    static const packet_detail* get(const ReceivePacketTable& head);
    
private:
    PacketDatabase();
    
    void init();
    
    static PacketDatabase& instance();
    
    std::unordered_map<ReceivePacketTable, packet_detail> packet_map;
};