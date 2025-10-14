#pragma once

#include "packets/DeserializeHandler.h"

class VenderFound : public DeserializeHandler
{
public:

    void deserialize_internal(const PacketInfo pk_header) override;

private:

    uint32_t account_id = 0;
    std::string shop_name;
    
};
