#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>
#include <ctpl_stl.h>
#include <nlohmann/json.hpp>

#include "PackeTable.h"

class DeserializeHandler
{
public:
    virtual ~DeserializeHandler() = default;

    void deserialize(const uint32_t in_pid, const std::vector<uint8_t>* data);

    static nlohmann::json get_app_config();
    
    virtual void deserialize_internal(const PacketInfo pk_header) = 0;

protected:
    
    void debug_packet() const;

    static std::string string_to_hex(const std::string& input);

    void send_request(const std::string& endpoint, nlohmann::json& in_data) const;
    
    std::span<const uint8_t> pkt_data;
    
    uint16_t pid = 0;

private:
    
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
    
    static ctpl::thread_pool curl_pool;
};

using HandlerFactory = std::function<std::unique_ptr<DeserializeHandler>()>;