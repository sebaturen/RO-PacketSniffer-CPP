#pragma once
#include "packets/DeserializeHandler.h"

class SpecialItemObtain : public DeserializeHandler
{
public:
    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    void report_special_item_obtain();

    uint16_t type = 0;
    uint32_t received_item_id = 0;
    uint32_t from_item_id = 0;
    std::string character_name;
    std::string from_monster_code;
};
