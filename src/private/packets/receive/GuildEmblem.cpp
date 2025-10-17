#include "packets/receive/GuildEmblem.h"

void GuildEmblem::deserialize_internal(const ReceivePacketTable pk_header)
{
    guild_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);

    int a = 10;
    
}
