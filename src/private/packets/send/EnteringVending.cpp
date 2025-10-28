#include "packets/send/EnteringVending.h"

std::vector<uint8_t> EnteringVending::GetPayload()
{
    std::vector<uint8_t> buffer;

    uint32_t account_id = 1378769; // example

    // Big-endian
    buffer.push_back((account_id >> 24) & 0xFF);
    buffer.push_back((account_id >> 16) & 0xFF);
    buffer.push_back((account_id >> 8) & 0xFF);
    buffer.push_back(account_id & 0xFF);
    
    return buffer;
}
