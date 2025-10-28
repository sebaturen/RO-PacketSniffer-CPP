#pragma once
#include "packets/DeserializeHandler.h"

class OfflineCloneFound : public DeserializeHandler
{
public:

    void deserialize_internal(const ReceivePacketTable pk_header) override;

private:

    void report_clone();

    uint32_t clone_id = 0;
    uint32_t job_id = 0;
    uint32_t coord_x = 0;
    uint32_t coord_y = 0;
    uint16_t sex = -1;
    std::string name;
    std::string map;
};
