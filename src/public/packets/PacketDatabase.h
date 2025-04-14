#pragma once

#include <string>
#include <unordered_map>

struct packet_detail
{
    size_t size;
    std::string description;
};

class PacketDatabase
{
public:    
    static const packet_detail* get(const std::string& id);
    
private:
    PacketDatabase();
    
    void init();
    static PacketDatabase& instance();
    
    std::unordered_map<std::string, packet_detail> packet_map;
};