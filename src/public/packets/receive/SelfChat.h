#include <cstdint>

#include "../DeserializeHandler.h"

class SelfChat : public DeserializeHandler {
public:
    void deserialize(const uint8_t* data, size_t len) const override;
};

static const SelfChat g_selfChatHandler;