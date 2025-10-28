#pragma once

#include "packets/DeserializeHandler.h"

class VenderFound : public DeserializeHandler
{
public:

    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    void report_shop();

    uint32_t account_id = 0;
    std::string shop_name;
    std::string shop_map;
    
};
