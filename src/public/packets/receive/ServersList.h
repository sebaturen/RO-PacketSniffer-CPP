#pragma once
#include "packets/DeserializeHandler.h"

class ServersList : public DeserializeHandler
{
public:
    
    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    uint32_t account_id = 0;
};
