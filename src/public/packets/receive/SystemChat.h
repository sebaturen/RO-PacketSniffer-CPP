#pragma once
#include "packets/DeserializeHandler.h"

class SystemChat : public DeserializeHandler
{
public:
    void deserialize_internal(const PacketInfo pk_header) override;
};
