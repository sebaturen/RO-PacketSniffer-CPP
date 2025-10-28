#include "packets/receive/MapChanged.h"

#include "gameplay/character/Character.h"

void MapChanged::deserialize_internal(const ReceivePacketTable pk_header)
{
	map_name = std::string(reinterpret_cast<const char*>(pkt_data.data()));
	coord_x = pkt_data[16] | (pkt_data[17] << 8);
	coord_y = pkt_data[18] | (pkt_data[19] << 8);
	ip = std::string(reinterpret_cast<const char*>(pkt_data.data() + 20));
	port = pkt_data[24] | (pkt_data[25] << 8);
	url = std::string(reinterpret_cast<const char*>(pkt_data.data() + 26));

	Character::set_map(pid, map_name);
}