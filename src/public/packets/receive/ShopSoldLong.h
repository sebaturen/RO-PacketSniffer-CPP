#pragma once
#include "packets/DeserializeHandler.h"

class ShopSoldLong : public DeserializeHandler
{
public:

    void deserialize_internal(const PacketInfo pk_header) override;

private:

    void report_sold();

    uint16_t count = 0;
    uint32_t char_id = 0;
    uint32_t timestamp = 0;
    uint32_t amount = 0;
    
};
