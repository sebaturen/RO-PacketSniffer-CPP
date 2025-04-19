#pragma once

#include <vector>

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;
    
    virtual void deserialize(const std::vector<unsigned char> data) const = 0;
};
