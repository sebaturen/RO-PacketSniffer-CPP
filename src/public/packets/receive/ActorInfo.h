#pragma once

#include "../DeserializeHandler.h"

enum class ActorType : int8_t
{
    UNKNOWN = -1,
    PLAYER = 0,
    MONSTER = 5,
    NPC = 6
};

class ActorInfo : public DeserializeHandler
{
public:
    void deserialize_internal(const PacketInfo pk_header) override;

private:

    void actor_connected();
    void actor_moved();
    void actor_exists();

    // Actor info
    ActorType actor_type = ActorType::UNKNOWN;
    uint16_t account_id = 0;
    uint16_t character_id = 0;
    uint8_t job_id = 0;
};

static const ActorInfo g_actor;