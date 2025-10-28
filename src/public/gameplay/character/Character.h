#pragma once
#include <string>
#include <unordered_map>

class Character
{
public:
	// require refactor...

	static bool get_map(uint32_t pid, std::string& output);
	static void set_map(uint32_t pid, const std::string& map_name);

private:
	
	static std::pmr::unordered_map<uint32_t, std::string> map_location;

};
