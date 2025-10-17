#pragma once
#include "packets/DeserializeHandler.h"

enum UnitLevelUpType
{
    UNKOWN = -1,
    BASE = 0,
    JOB = 1,
};

class UnitLevelUp : public DeserializeHandler
{
public:

    void deserialize_internal(const ReceivePacketTable pk_header) override;

    UnitLevelUpType get_type() const { return type; }

private:

    uint32_t account_id = 0;
    UnitLevelUpType type = UnitLevelUpType::UNKOWN;
    
};
