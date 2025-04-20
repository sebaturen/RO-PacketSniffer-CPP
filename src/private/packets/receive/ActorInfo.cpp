#include "../../../public/packets/receive/ActorInfo.h"

#include <iostream>

void ActorInfo::deserialize_internal(const PacketInfo pk_header)
{
    /**
     * Base struct replicated from diferents packages
     * `v C a4 a4 v3 V v2 V2`
    */
    actor_type = static_cast<ActorType>(pkt_data[read_position++]);
    account_id = pkt_data[read_position++] | (pkt_data[read_position++] << 8) | (pkt_data[read_position++] << 16) | (pkt_data[read_position++] << 24);
    character_id = pkt_data[read_position++] | (pkt_data[read_position++] << 8) | (pkt_data[read_position++] << 16) | (pkt_data[read_position++] << 24);
    read_position = 18; // Skip to byte 18, start job position
    job_id = pkt_data[read_position++] | (pkt_data[read_position++] << 8);
    
    if (pk_header == PacketInfo::ACTOR_EXISTS_8)
    {
        actor_exists();
    }
    if (pk_header == PacketInfo::ACTOR_CONNECTED_8)
    {
        actor_connected();
    }
    if (pk_header == PacketInfo::ACTOR_MOVED_8)
    {
        actor_moved();
    }    
}

void ActorInfo::actor_connected()
{
    
}

void ActorInfo::actor_moved()
{
    
}

void ActorInfo::actor_exists()
{
    
}
