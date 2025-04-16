#include <cstdint>

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;
    
    virtual void deserialize(const uint8_t* data, size_t len) const = 0;
};
