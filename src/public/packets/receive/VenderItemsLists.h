#pragma once
#include "packets/DeserializeHandler.h"

struct VenderItem
{    
    uint32_t item_id = 0;
    uint16_t type = 0;
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
    uint32_t unknown_value = 0;
    uint32_t price = 0;
    uint32_t quantity = 0;
    uint32_t position = 0;
};

class VenderItemsLists : public DeserializeHandler
{
public:
    
    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    void report_vendor_shop();

    uint32_t vendor_id = 0;
    uint32_t vendor_cid = 0;
    uint32_t flag = 0;
    uint32_t expired_date = 0;
    std::vector<VenderItem> items;
    std::string map;
};
