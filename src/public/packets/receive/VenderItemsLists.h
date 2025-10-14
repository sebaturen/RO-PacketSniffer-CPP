#pragma once
#include "packets/DeserializeHandler.h"

struct VenderItem
{    
    uint32_t price = 0;
    uint32_t quantity = 0;
    uint32_t position = 0;
    uint16_t type = 0;
    uint32_t item_id = 0;
    uint16_t refine = 0;
    uint32_t card_slot_1 = 0;
    uint32_t card_slot_2 = 0;
    uint32_t card_slot_3 = 0;
    uint32_t card_slot_4 = 0;
    uint32_t enchant_slot_1 = 0;
    uint32_t enchant_slot_1_val = 0;
    uint32_t enchant_slot_2 = 0;
    uint32_t enchant_slot_2_val = 0;
    uint32_t enchant_slot_3 = 0;
    uint32_t enchant_slot_3_val = 0;    
    uint32_t enchant_slot_4 = 0;
    uint32_t enchant_slot_4_val = 0;
    std::string unknown_part;
};

class VenderItemsLists : public DeserializeHandler
{
public:
    
    void deserialize_internal(const PacketInfo pk_header) override;

private:

    uint32_t account_id = 0;
    uint32_t shop_id = 0;
    std::vector<VenderItem> vendor_items;
};
