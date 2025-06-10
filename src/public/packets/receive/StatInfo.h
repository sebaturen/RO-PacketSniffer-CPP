#pragma once

#include "../DeserializeHandler.h"

enum class StatType : uint32_t
{
    UNKNOWN = 0,
    // CHANGE_BASE_LEVEL = 11,
    // CHANGE_JOB_LEVEL = 12,
    // Stat INFO 7
    CURRENT_BASE_EXP = 700, //1+(100*7),
    CURRENT_JOB_EXP = 702,  //2+(100*7),
    BASE_LVL = 711,         //11+(100*7),
    TOTAL_BASE_EXP = 722,   //22+(100*7),
    TOTAL_JOB_EXP = 723,    //23+(100*7),
    JOB_LVL = 755,          //55+(100*7)
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
