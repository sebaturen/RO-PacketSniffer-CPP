#pragma once

#include <span>
#include <vector>
#include "PacketDatabase.h"

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;

    void deserialize(const std::vector<uint8_t>* data);
    
    virtual void deserialize_internal(const PacketInfo pk_header) = 0;

protected:
    
    void debug_packet() const;
        
    uint16_t read_position = 0;

    std::span<const uint8_t> pkt_data;
};
