#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>
#include <nlohmann/json_fwd.hpp>

#include "PackeTable.h"

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;

    void deserialize(const std::vector<uint8_t>* data);

    static void set_app_config(const nlohmann::json& in_app_config);
    
    virtual void deserialize_internal(const PacketInfo pk_header) = 0;

protected:
    
    void debug_packet() const;

    static void send_request(const std::string& endpoint, const nlohmann::json& data);
    
    std::span<const uint8_t> pkt_data;

private:

    static nlohmann::json app_config;
};

using HandlerFactory = std::function<std::unique_ptr<DeserializeHandler>()>;