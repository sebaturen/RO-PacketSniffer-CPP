#pragma once

#include "../DeserializeHandler.h"

enum class StatType : uint16_t
{
    UNKNOWN = 0,
    CURRENT_BASE_EXP = 1,
    CURRENT_JOB_EXP = 2,
    BASE_LVL = 11,
    TOTAL_BASE_EXP = 22,
    TOTAL_JOB_EXP = 23,
    JOB_LVL = 55
};

class StatInfo : public DeserializeHandler
{
public:
    void deserialize_internal(const PacketInfo pk_header) override;

    StatType get_type() const { return type; }
    uint64_t get_value() const { return val; }
    
private:

    StatType type = StatType::UNKNOWN;
    uint64_t val = 0;
};
