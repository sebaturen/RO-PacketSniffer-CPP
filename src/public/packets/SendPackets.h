#pragma once
#include <cstdint>
#include <vector>

#include "PacketDatabase.h"
#include "SendPacketTable.h"

class SendPackets
{
public:
    virtual ~SendPackets() = default;
    explicit SendPackets(SendPacketTable InPacketHeader) : packetHeader(InPacketHeader) {} 

    void Send();

protected:

    virtual std::vector<uint8_t> GetPayload() = 0;

private:

    SendPacketTable packetHeader = SendPacketTable::UNKNOWN;
};
