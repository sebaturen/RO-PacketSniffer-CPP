#include "packets/receive/UnitLevelUp.h"

#include "gameplay/exp_calculator/ExpCalculator.h"

void UnitLevelUp::deserialize_internal(const PacketInfo pk_header)
{
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    uint32_t type_id = pkt_data[4] | (pkt_data[5] << 8) | (pkt_data[6] << 16) | (pkt_data[7] << 24);
    type = static_cast<UnitLevelUpType>(type_id);

    if (ExpCharacter* c = ExpCalculator::get_char(port))
    {
        c->level_up(*this);
    }
}
