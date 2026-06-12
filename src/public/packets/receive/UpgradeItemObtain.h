#pragma once
#include "packets/DeserializeHandler.h"

class UpgradeItemObtain : public DeserializeHandler
{
public:
    void deserialize_internal(const ReceivePacketTable pk_header) override;
    
private:
    
    void report_upgrade_item();
    
    std::string character_name;
    uint32_t item_id = 0;
    uint32_t level = 0;
    uint32_t unknown_val = 0;
};
