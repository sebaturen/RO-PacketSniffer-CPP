#include "packets/receive/VenderItemsLists.h"

#include <iostream>

#include "gameplay/character/Character.h"

namespace VenderItemsListsAPI
{
    constexpr const char* VENDER_SHOP_ITEMS_API = "vending/items";
}

void VenderItemsLists::deserialize_internal(const ReceivePacketTable pk_header)
{
    size_t item_list_start = 0;
    if (pk_header == ReceivePacketTable::VENDER_ITEMS_LIST_4)
    {
        vendor_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
        vendor_cid = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
        flag = pkt_data[8];
        expired_date = pkt_data[9] | (pkt_data[10] << 8) | (pkt_data[11] << 16) | (pkt_data[12] << 24);
        item_list_start = 13;
    }
    else if (pk_header == ReceivePacketTable::VENDER_ITEMS_LIST_3)
    {
        vendor_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
        vendor_cid = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
        item_list_start = 8;
    }
    else
    {
        return;
    }

    size_t i = item_list_start;
    while (i < pkt_data.size())
    {
        VenderItem item;
        item.price = pkt_data[i] | (pkt_data[i+1] << 8) | (pkt_data[i+2] << 16) | (pkt_data[i+3] << 24);
        item.quantity =  pkt_data[i+4] | (pkt_data[i+5] << 8);
        item.position = pkt_data[i+6] | (pkt_data[i+7] << 8);
        item.type = pkt_data[i+8];
        item.item_id = pkt_data[i+9] | (pkt_data[i+10] << 8) | (pkt_data[i+11] << 16) | (pkt_data[i+12] << 24);
        item.unknown_value = pkt_data[i+13] | (pkt_data[i+14] << 8);
        
        size_t cs = i+14; // Card Slot
        item.card_slot_1 = pkt_data[cs+1] | (pkt_data[cs+2] << 8) | (pkt_data[cs+3] << 16) | (pkt_data[cs+4] << 24);
        item.card_slot_2 = pkt_data[cs+5] | (pkt_data[cs+6] << 8) | (pkt_data[cs+7] << 16) | (pkt_data[cs+8] << 24);
        item.card_slot_3 = pkt_data[cs+9] | (pkt_data[cs+10] << 8) | (pkt_data[cs+11] << 16) | (pkt_data[cs+12] << 24);
        item.card_slot_4 = pkt_data[cs+13] | (pkt_data[cs+14] << 8) | (pkt_data[cs+15] << 16) | (pkt_data[cs+16] << 24);
        
        size_t es = cs+16; // Enchant slot
        item.enchant_slot_1 = pkt_data[es+1] | (pkt_data[es+2] << 8);
        item.enchant_slot_1_val = pkt_data[es+3] | (pkt_data[es+4] << 8) | (pkt_data[es+5] << 16);
        item.enchant_slot_2 = pkt_data[es+6] | (pkt_data[es+7] << 8);
        item.enchant_slot_2_val = pkt_data[es+8] | (pkt_data[es+9] << 8) | (pkt_data[es+10] << 16);
        item.enchant_slot_3 = pkt_data[es+11] | (pkt_data[es+12] << 8);
        item.enchant_slot_3_val = pkt_data[es+13] | (pkt_data[es+14] << 8) | (pkt_data[es+15] << 16);
        item.enchant_slot_4 = pkt_data[es+16] | (pkt_data[es+17] << 8);
        item.enchant_slot_4_val = pkt_data[es+18] | (pkt_data[es+19] << 8) | (pkt_data[es+20] << 16);
        
        // skip enchant-to-refine unknown values
        size_t uk = es+20;
        size_t uk_end = uk+11;
        for (; uk < uk_end; ++uk) {
            char buffer[4];
            int ed = sprintf_s(buffer, sizeof(buffer), "%02X ", pkt_data[uk+1]);
            if (ed > 0)
                item.unknown_part += buffer;
        }
        
        size_t r = uk_end;
        item.refine = pkt_data[r+1] | (pkt_data[r+2] << 8);
        i = r+3;

        items.push_back(item);
    }

    if (Character::get_map(pid, map))
    {
        report_vendor_shop();
    }
}

void VenderItemsLists::report_vendor_shop()
{
    nlohmann::json shop_items;
    for (const auto& item : items)
    {
        nlohmann::json api_item = {
            { "item_id", item.item_id },
            { "type", item.type },
            { "refine", item.refine },
            { "card_slot_1", item.card_slot_1 },
            { "card_slot_2", item.card_slot_2 },
            { "card_slot_3", item.card_slot_3 },
            { "card_slot_4", item.card_slot_4 },
            { "enchant_slot_1", item.enchant_slot_1 },
            { "enchant_slot_1_val", item.enchant_slot_1_val },
            { "enchant_slot_2", item.enchant_slot_2 },
            { "enchant_slot_2_val", item.enchant_slot_2_val },
            { "enchant_slot_3", item.enchant_slot_3 },
            { "enchant_slot_3_val", item.enchant_slot_3_val },
            { "enchant_slot_4", item.enchant_slot_4 },
            { "enchant_slot_4_val", item.enchant_slot_4_val },
            { "unknown_part", string_to_hex(item.unknown_part) },
            { "unknown_part_val", item.unknown_value },
            { "price", item.price },
            { "quantity", item.quantity },
            { "position", item.position }
        };

        shop_items.push_back(api_item);
    }
    
    nlohmann::json data = {
        {"vendor_id", vendor_id},
        {"vendor_cid", vendor_cid},
        {"flag", flag},
        {"expired_date", expired_date},
        {"shop_items", shop_items}
    };
    
    send_request(VenderItemsListsAPI::VENDER_SHOP_ITEMS_API, data);
}
