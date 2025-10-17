#pragma once

#include "packets/DeserializeHandler.h"

class ReceivedCharacter
{
public:
    ReceivedCharacter(
        uint32_t character_id,
        uint32_t base_level,
        uint32_t job_level,
        std::string name,
        uint64_t current_base_exp,
        uint64_t current_job_exp)
        :
            character_id(character_id),
            base_level(base_level),
            job_level(job_level),
            name(std::move(name)),
            current_base_exp(current_base_exp),
            current_job_exp(current_job_exp)
    {
    }

    uint32_t get_character_id() const { return character_id; }
    uint32_t get_base_level() const { return base_level; }
    uint32_t get_job_level() const { return job_level; }
    std::string get_name() const { return name; }
    uint64_t get_current_base_exp() const { return current_base_exp; }
    uint64_t get_current_job_exp() const { return current_job_exp; }
    
private:
    
    uint32_t character_id = 0;
    uint64_t current_base_exp = 0;
    uint64_t current_job_exp = 0;
    uint32_t base_level = 0;
    uint32_t job_level = 0;
    std::string name;
    
};

class ReceivedCharacters : public DeserializeHandler
{
public:
    
    void deserialize_internal(const ReceivePacketTable pk_header) override;
    const std::vector<ReceivedCharacter>& get_characters() const { return characters; }
    void add_or_update_character(const ReceivedCharacter& in_character);

private:
    
    std::vector<ReceivedCharacter> characters;
    
};
