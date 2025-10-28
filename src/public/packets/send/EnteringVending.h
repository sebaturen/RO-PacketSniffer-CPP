#pragma once
#include "packets/SendPackets.h"
#include "packets/SendPacketTable.h"

class EnteringVending : public SendPackets
{
public:
    EnteringVending() : SendPackets(SendPacketTable::ENTERING_VENDING) {}

protected:
    
    virtual std::vector<uint8_t> GetPayload() override;
    
};
