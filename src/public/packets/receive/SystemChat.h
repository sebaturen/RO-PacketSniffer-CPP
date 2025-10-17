#pragma once
#include "packets/DeserializeHandler.h"

class SystemChat : public DeserializeHandler
{
public:
    void deserialize_internal(const ReceivePacketTable pk_header) override;
};
