#pragma once
#include "packets/DeserializeHandler.h"

class MapChanged : public DeserializeHandler
{
public:
	void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

	std::string map_name;
	uint32_t coord_x = 0;
	uint32_t coord_y = 0;
	std::string ip;
	uint32_t port = 0;
	std::string url;

};
