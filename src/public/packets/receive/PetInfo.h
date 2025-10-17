#pragma once
#include "packets/DeserializeHandler.h"

enum PetInfoValue
{
    HUNGRY = 0x02,
    ACC_UNEQUIP = 0x03,
    PERFORMANCE = 0x04
};

class PetInfo : public DeserializeHandler
{
public:
    
    void deserialize_internal(const PacketInfo pk_header) override;

private:

    std::string name;
    uint16_t rename_flag = 0;
    uint32_t level = 0;
    uint32_t hungry = 0;
    uint32_t friendly = 0;
    uint32_t accessory_id = 0;
    uint32_t type_id = 0;
    
};
