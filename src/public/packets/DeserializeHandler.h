#pragma once

#include <span>
#include <vector>
#include "PacketDatabase.h"

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;
    
    virtual void deserialize(const PacketInfo pk_info, const std::span<const uint8_t>* data) = 0;
};
