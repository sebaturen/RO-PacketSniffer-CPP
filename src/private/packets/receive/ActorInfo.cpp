#include "packets/receive/ActorInfo.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "gameplay/character/Character.h"

namespace ActorInfoAPI
{
    constexpr const char* PLAYER_API_ENDPOINT = "character";
    constexpr const char* PARTY_GUILD_API_ENDPOINT = "party_guild";
    constexpr const char* MONSTER_API_ENDPOINT = "monster";
}

void ActorInfo::deserialize_internal(const ReceivePacketTable pk_header)
{
    if (pk_header == ReceivePacketTable::ACTOR_MOVED_8 ||
        pk_header == ReceivePacketTable::ACTOR_CONNECTED_8 || 
        pk_header == ReceivePacketTable::ACTOR_EXISTS_8)
    {
        deserialize_extended(pk_header);
    }
    
    if (pk_header == ReceivePacketTable::ACTOR_INFO_2)
    {
        deserialize_minimal();
    }
}

/*
 * Contain a extended version of the actor info packet.
 */
void ActorInfo::deserialize_extended(const ReceivePacketTable pk_header)
{
    int8_t offset_one = 0;
    int8_t offset_two = 0;
    if (pk_header == ReceivePacketTable::ACTOR_CONNECTED_8)
    {
        offset_two = -1;
    }
    if (pk_header == ReceivePacketTable::ACTOR_MOVED_8)
    {
        offset_one = 4;
        offset_two = 6;
    }
    
    /**
     * Packet structure:
    */
    actor_type = static_cast<ActorType>(pkt_data[0]);
    actor_id = pkt_data[1] | (pkt_data[2] << 8) | (pkt_data[3] << 16) | (pkt_data[4] << 24);
    character_id = pkt_data[5] | (pkt_data[6] << 8) | (pkt_data[7] << 16) | (pkt_data[8] << 24);
    
    type_id = pkt_data[19] | (pkt_data[20] << 8);
    hair_style_id = pkt_data[21] | (pkt_data[22] << 8);
    weapon_id = pkt_data[23] | (pkt_data[24] << 8) | (pkt_data[25] << 16) | (pkt_data[26] << 24);
    shield_id = pkt_data[27] | (pkt_data[28] << 8) | (pkt_data[29] << 16) | (pkt_data[30] << 24);
    low_head_id = pkt_data[31] | (pkt_data[32] << 8);
    top_head_id = pkt_data[33 + offset_one] | (pkt_data[34 + offset_one] << 8);
    mid_head_id = pkt_data[35 + offset_one] | (pkt_data[36 + offset_one] << 8);
    hair_color_id = pkt_data[37 + offset_one] | (pkt_data[38 + offset_one] << 8);
    clothes_color_id = pkt_data[39 + offset_one] | (pkt_data[40 + offset_one] << 8);
    
    guild_id = pkt_data[45 + offset_one] | (pkt_data[46 + offset_one] << 8) | (pkt_data[47 + offset_one] << 16) | (pkt_data[48 + offset_one] << 24);
    guild_emblem_id = pkt_data[49 + offset_one] | (pkt_data[50 + offset_one] << 8) | (pkt_data[51 + offset_one] << 16) | (pkt_data[52 + offset_one] << 24);
    sex = pkt_data[58 + offset_one];
    
    uint32_t coords = 0; // big endian
    coords |= static_cast<uint32_t>(pkt_data[59 + offset_one])     << 16;
    coords |= static_cast<uint32_t>(pkt_data[59 + offset_one + 1]) << 8;
    coords |= static_cast<uint32_t>(pkt_data[59 + offset_one + 2]);
    coord_x = coords >> 14;
    coord_y = (coords >> 4) & 0x3FF;
    Character::get_map(pid, coord_map);
    
    level = pkt_data[65 + offset_two] | (pkt_data[66 + offset_two] << 8) | (pkt_data[67 + offset_two] << 16) | (pkt_data[68 + offset_two] << 24);
    clothes_style = pkt_data[78 + offset_two] | (pkt_data[79 + offset_two] << 8);
    name = std::string(reinterpret_cast<const char*>(pkt_data.data() + (80 + offset_two)), pkt_data.size() - (80 + offset_two));

    if (actor_type == ActorType::PLAYER)
    {
        report_player();
    }
    if (actor_type == ActorType::MONSTER)
    {
        report_monster();
    }
}

/*
 * Small version of actor, use only for players, and containe:
 *  - Name
 *  - PartyName
 *  - GuildName
 *  - GuildTitle
 *  - TitleId
 */
void ActorInfo::deserialize_minimal()
{
    actor_id = pkt_data[0] | (pkt_data[1] << 8) | (pkt_data[2] << 16) | (pkt_data[3] << 24);
    
    name         = std::string(reinterpret_cast<const char*>(pkt_data.data() + 4));
    party_name   = std::string(reinterpret_cast<const char*>(pkt_data.data() + 28));
    guild_name   = std::string(reinterpret_cast<const char*>(pkt_data.data() + 52));
    guild_title  = std::string(reinterpret_cast<const char*>(pkt_data.data() + 76));

    report_player_minimal();
}

void ActorInfo::report_player()
{
    nlohmann::json customization = {
        {"hair_style_id", hair_style_id},
        {"hair_color_id", hair_color_id},
        {"weapon_id", weapon_id},
        {"shield_id", shield_id},
        {"top_head_id", top_head_id},
        {"mid_head_id", mid_head_id},
        {"low_head_id", low_head_id},
        {"clothes_color_id", clothes_color_id},
        {"clothes_style", clothes_style}
    };

    nlohmann::json guild_info = {
        {"id", guild_id},
        {"emblem_id", guild_emblem_id}
    };

    nlohmann::json char_info = {
        {"job_id", type_id},
        {"sex", sex},
        {"level", level},
        {"name", string_to_hex(name) }
    };

    nlohmann::json location = {
        {"coord_map", string_to_hex(coord_map)},
        {"coord_x", coord_x},
        {"coord_y", coord_y}
    };

    nlohmann::json data = {
        {"account_id", actor_id},
        {"character_id", character_id},
        {"info", char_info},
        {"customization", customization},
        { "guild", guild_info },
        {"location", location}
    };

    send_request(ActorInfoAPI::PLAYER_API_ENDPOINT, data);
}

void ActorInfo::report_monster()
{
    nlohmann::json data = {
        {"id", type_id},
        {"level", level},
        {"coord_x", coord_x},
        {"coord_y", coord_y}
    };

    if (type_id == 2288)
    {
        std::cout << "/navi gld2_ald " << coord_x << "/" << coord_y << std::endl;
    }
    //std::cout << "[DEBUG] Reporting monster: " << data.dump() << std::endl;

    //... send_request(data); <-- TODO: get map!
}

void ActorInfo::report_player_minimal()
{
    nlohmann::json data = {
        {"account_id", actor_id},
        {"name", string_to_hex(name) },
        {"party", string_to_hex(party_name) },
        {"guild", string_to_hex(guild_name) },
        {"guild_title", string_to_hex(guild_title) }
    };

    send_request(ActorInfoAPI::PARTY_GUILD_API_ENDPOINT, data);
}
