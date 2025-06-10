#include "packets/receive/SyncRequest.h"

#include "gameplay/exp_calculator/ExpCalculator.h"

void SyncRequest::deserialize_internal(const PacketInfo pk_header)
{
    account_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);

    ExpCalculator::sync_account(pid, account_id);
}
