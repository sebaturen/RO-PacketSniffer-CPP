#include "gameplay/character/Character.h"

#include <iostream>
#include <shared_mutex>

static std::shared_mutex map_mutex_;
std::pmr::unordered_map<uint32_t, std::string> Character::map_location;

bool Character::get_map(uint32_t pid, std::string& output)
{
	std::shared_lock lock(map_mutex_);
	if (const auto it = map_location.find(pid); it != map_location.end())
	{
		output = it->second;
		return true;
	}
	
	std::cout << "Map not found [pid: "<< pid <<"]" << std::endl;
	
	return false; 
}

void Character::set_map(uint32_t pid, const std::string& map_name)
{
	std::unique_lock lock(map_mutex_);
	map_location.insert_or_assign(pid, map_name);
}
