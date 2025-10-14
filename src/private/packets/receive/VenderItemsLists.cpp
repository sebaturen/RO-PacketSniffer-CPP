#include "packets/receive/VenderItemsLists.h"

void VenderItemsLists::deserialize_internal(const PacketInfo pk_header)
{
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    shop_id = pkt_data[4] | (pkt_data[5] << 8 ) | (pkt_data[6] << 16) | (pkt_data[7] << 24);

    size_t i = 8;
    while (i < pkt_data.size())
    {
        VenderItem item;
        item.price = pkt_data[i] | (pkt_data[i+1] << 8) | (pkt_data[i+2] << 16) | (pkt_data[i+3] << 24);
        item.quantity =  pkt_data[i+4] | (pkt_data[i+5] << 8);
        item.position = pkt_data[i+6] | (pkt_data[i+7] << 8);
        item.type = pkt_data[i+8];
        item.item_id = pkt_data[i+9] | (pkt_data[i+10] << 8) | (pkt_data[i+11] << 16) | (pkt_data[i+12] << 24);
        // skip 13-14 unknown values
        item.refine = pkt_data[i+15];
        item.card_slot_1 = pkt_data[i+16] | (pkt_data[i+17] << 8) | (pkt_data[i+18] << 16) | (pkt_data[i+19] << 24);
        item.card_slot_2 = pkt_data[i+20] | (pkt_data[i+21] << 8) | (pkt_data[i+22] << 16) | (pkt_data[i+23] << 24);
        item.card_slot_3 = pkt_data[i+24] | (pkt_data[i+25] << 8) | (pkt_data[i+26] << 16) | (pkt_data[i+27] << 24);
        item.card_slot_4 = pkt_data[i+28] | (pkt_data[i+29] << 8) | (pkt_data[i+30] << 16) | (pkt_data[i+31] << 24);
        item.enchant_slot_1 = pkt_data[i+32] | (pkt_data[i+33] << 8);
        item.enchant_slot_1_val = pkt_data[i+34] | (pkt_data[i+35] << 8) | (pkt_data[i+36] << 16);
        item.enchant_slot_2 = pkt_data[i+37] | (pkt_data[i+38] << 8);
        item.enchant_slot_2_val = pkt_data[i+39] | (pkt_data[i+40] << 8) | (pkt_data[i+41] << 16);
        item.enchant_slot_3 = pkt_data[i+42] | (pkt_data[i+43] << 8);
        item.enchant_slot_3_val = pkt_data[i+44] | (pkt_data[i+45] << 8) | (pkt_data[i+46] << 16);
        item.enchant_slot_4 = pkt_data[i+47] | (pkt_data[i+48] << 8);
        item.enchant_slot_4_val = pkt_data[i+49] | (pkt_data[i+50] << 8) | (pkt_data[i+51] << 16);
        i = i+51;

        // final Unknown section
        size_t final_size = i + 11;
        for (; i < final_size; ++i) {
            char buffer[4];
            int ed = sprintf_s(buffer, sizeof(buffer), "%02X ", pkt_data[i]);
            if (ed > 0)
                item.unknown_part += buffer;
        }
        i++;

        vendor_items.push_back(item);
    }
}
