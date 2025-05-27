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

    uint32_t get_account_id() const { return account_id; }
    ExpType get_type() const { return type; }
    int64_t get_exp() const { return exp; }

private:
    
    uint32_t account_id = 0;
    int64_t exp = 0;
    ExpType type = ExpType::UNKNOWN;
};
