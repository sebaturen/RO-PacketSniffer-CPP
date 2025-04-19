#include <cstdint>

#include "../DeserializeHandler.h"

class SelfChat : public DeserializeHandler {
public:
    void deserialize(const std::vector<unsigned char> data) const override;
};

static const SelfChat g_selfChatHandler;