#pragma once

#include "../DeserializeHandler.h"

enum class ExpType : int8_t
{
    UNKNOWN = -1,
    BASE = 1,
    JOB = 2    
};

class Exp : public DeserializeHandler
{
public:
    void deserialize_internal(const PacketInfo pk_header) override;

private:
    
    uint32_t account_id = 0;
    int64_t exp = 0;
    ExpType type = ExpType::UNKNOWN;
};
