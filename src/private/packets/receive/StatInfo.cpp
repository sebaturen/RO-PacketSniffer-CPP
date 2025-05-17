#include "../../../public/packets/receive/StatInfo.h"

#include <iostream>

void StatInfo::deserialize_internal(const PacketInfo pk_header)
{
    type = static_cast<StatType>(pkt_data[0] | (pkt_data[1] << 8));
    val =
        static_cast<int64_t>(pkt_data[2]) |
        (static_cast<int64_t>(pkt_data[3]) << 8) |
        (static_cast<int64_t>(pkt_data[4]) << 16) |
        (static_cast<int64_t>(pkt_data[5]) << 24) |
        (static_cast<int64_t>(pkt_data[6]) << 32) |
        (static_cast<int64_t>(pkt_data[7]) << 40) |
        (static_cast<int64_t>(pkt_data[8]) << 48) |
        (static_cast<int64_t>(pkt_data[9]) << 56);

    if (
          type == StatType::CURRENT_BASE_EXP ||
          type == StatType::CURRENT_JOB_EXP ||
          type == StatType::BASE_LVL  ||
          type == StatType::TOTAL_BASE_EXP  ||
          type == StatType::TOTAL_JOB_EXP  ||
          type == StatType::JOB_LVL
        )
    {
        std::cout << "[INFO] Stat Info. Exp: " << val << " Type: " << static_cast<int>(type) << '\n';
    }
}
