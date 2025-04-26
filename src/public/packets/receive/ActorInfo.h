#pragma once

#include <string>

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

    void deserialize_extended(const PacketInfo pk_header);
    void deserialize_minimal();
    
    void report_player();
    void report_monster();
    void report_player_minimal();

    // Actor info
    ActorType actor_type = ActorType::UNKNOWN;
    uint32_t actor_id = 0; // [MonsterMapId for monsters] or [Account ID for player]
    uint32_t character_id = 0;
    uint32_t type_id = 0; // [MonsterId for monster] or [JobId for player]
    uint32_t hair_style_id = 0;
    uint32_t weapon_id = 0;
    uint32_t shield_id = 0;
    uint32_t low_head_id = 0;
    uint32_t top_head_id = 0;
    uint32_t mid_head_id = 0;
    uint32_t hair_color_id = 0;
    uint32_t clothes_color_id = 0;
    uint32_t guild_id = 0;
    uint32_t guild_emblem_id = 0;
    uint32_t sex = 0;
    uint32_t coord_x = 0;
    uint32_t coord_y = 0;
    uint32_t level = 0;
    
    std::string name;
    std::string party_name;
    std::string guild_name;
    std::string guild_title;
};