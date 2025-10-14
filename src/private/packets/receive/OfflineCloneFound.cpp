#include "packets/receive/OfflineCloneFound.h"

#include <iostream>

void OfflineCloneFound::deserialize_internal(const PacketInfo pk_header)
{
    clone_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    job_id = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
    name = std::string(reinterpret_cast<const char*>(pkt_data.data() + 35));

    coord_x = pkt_data[8] | (pkt_data[9] << 8);
    coord_y = pkt_data[10] | (pkt_data[11] << 8);
    
    //std::cout << "Offline clone found [" << clone_id <<"] " << name << " ("<< coord_x <<","<< coord_y <<")" << std::endl;
}
