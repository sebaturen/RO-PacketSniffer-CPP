#pragma once
#include "packets/DeserializeHandler.h"

class GuildEmblem : public DeserializeHandler
{
public:
    
    void deserialize_internal(const PacketInfo pk_header) override;

private:

    uint32_t guild_id = 0;
    uint32_t emblem_id = 0;
    std::string emblem;
    
};
