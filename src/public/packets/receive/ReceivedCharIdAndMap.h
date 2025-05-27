#pragma once
#include "packets/DeserializeHandler.h"

class ReceivedCharIdAndMap : public DeserializeHandler
{
public:
    
    void deserialize_internal(const PacketInfo pk_header) override;

private:
        
    uint32_t character_id = 0;
    std::string map_name;
    std::string map_url;
};
