#include "packets/receive/PetInfo.h"

#include <iostream>
#include "Sniffer.h"
#include "ding.h"

#include "gameplay/shop_sniffer/ShopSniffer.h"

#pragma comment(lib, "winmm.lib")

void PetInfo::deserialize_internal(const ReceivePacketTable pk_header)
{
    if (pk_header == ReceivePacketTable::PET_INFO)
    {        
        name = std::string(reinterpret_cast<const char*>(pkt_data.data()));
        rename_flag = pkt_data[24];
        level = pkt_data[25] | (pkt_data[26] << 8);
        hungry = pkt_data[27] | (pkt_data[28] << 8);
        friendly = pkt_data[29] | (pkt_data[30] << 8);
        accessory_id = pkt_data[31] | (pkt_data[32] << 8);
        type_id = pkt_data[33] | (pkt_data[34] << 8);
        std::cout << "PetInfo "<< name << " | Hungry: " << hungry << std::endl;
    }
    
    if (pk_header == ReceivePacketTable::PET_INFO2)
    {
        PetInfoValue value_type = static_cast<PetInfoValue>(pkt_data[0]);
        uint32_t pet_id = pkt_data[1] | (pkt_data[2] << 8) | pkt_data[3] | (pkt_data[4] << 16);
        uint32_t value = pkt_data[5] | (pkt_data[6] << 8) | pkt_data[7] | (pkt_data[8] << 16);
        if (value_type == HUNGRY)
        {
            hungry = value;
            if (hungry <= 25)
            {
                PlaySoundA(reinterpret_cast<LPCSTR>(sound_data), NULL, SND_MEMORY | SND_SYNC);
            }
            std::cout << "Pet Info update " << value_type << " | Hungry: " << hungry << std::endl;
        }
    }

}
