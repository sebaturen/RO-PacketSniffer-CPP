#include "../../../public/packets/receive/ActorInfo.h"

void ActorInfo::deserialize(const PacketInfo pk_info, const std::span<const uint8_t>* data)
{
    pkt_data = data;
    
    actor();
    if (pk_info == PacketInfo::ACTOR_EXISTS_8)
    {
        actor_exists();
    }
    if (pk_info == PacketInfo::ACTOR_CONNECTED_8)
    {
        actor_connected();
    }
    if (pk_info == PacketInfo::ACTOR_MOVED_8)
    {
        actor_moved();
    }    
}

void ActorInfo::actor()
{
    
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
