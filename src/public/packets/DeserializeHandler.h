#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>
#include <nlohmann/json_fwd.hpp>
#include <ctpl_stl.h>

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

    static std::string string_to_hex(const std::string& input);

    static void send_request(const std::string& endpoint, const std::string& data);
    
    std::span<const uint8_t> pkt_data;

private:

    static nlohmann::json app_config;
    static ctpl::thread_pool curl_pool;
};

using HandlerFactory = std::function<std::unique_ptr<DeserializeHandler>()>;