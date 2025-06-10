#include "packets/receive/CharacterStatus.h"

#include <iostream>

#include "gameplay/exp_calculator/ExpCalculator.h"

void CharacterStatus::deserialize_internal(const PacketInfo pk_header)
{
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);

    //ExpCalculator::sync_port(port, account_id); beware! this account_id is for character status, not your current character
}
