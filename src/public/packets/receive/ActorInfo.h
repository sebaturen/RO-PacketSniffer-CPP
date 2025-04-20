

#include "../DeserializeHandler.h"

class ActorInfo : public DeserializeHandler
{
public:
    virtual void deserialize(const PacketInfo pk_info, const std::span<const uint8_t>* data);

private:

    void actor();
    void actor_connected();
    void actor_moved();
    void actor_exists();
    
    const std::span<const uint8_t>* pkt_data = nullptr;

    // Actor info
    uint16_t account_id = 0;
    uint16_t character_id = 0;
    uint8_t job_id = 0;
};

static const ActorInfo g_actor_info;