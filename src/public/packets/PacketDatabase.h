#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

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
    const DeserializeHandler* handler;
    bool alert = false;
};

class PacketDatabase
{
public:    
    static const packet_detail* get(const uint16_t& head);
    static const std::unordered_set<uint16_t>& get_all_ids();
    
private:
    PacketDatabase();
    
    void init();
    static PacketDatabase& instance();
    
    std::unordered_map<uint16_t, packet_detail> packet_map;
    static std::unordered_set<uint16_t> s_valid_ids;
};