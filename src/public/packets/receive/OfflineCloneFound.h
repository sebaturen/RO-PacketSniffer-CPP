#pragma once
#include "packets/DeserializeHandler.h"

class OfflineCloneFound : public DeserializeHandler
{
public:

    void deserialize_internal(const PacketInfo pk_header) override;

private:

    uint32_t clone_id = 0;
    uint32_t job_id = 0;
    uint32_t coord_x = 0;
    uint32_t coord_y = 0;
    std::string name;
};
