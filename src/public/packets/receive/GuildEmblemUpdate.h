#pragma once
#include "packets/DeserializeHandler.h"

class GuildEmblemUpdate : public DeserializeHandler
{
public:
    
    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    uint32_t guild_id = 0;
    uint32_t emblem_id = 0;
    std::string emblem;
    
};
