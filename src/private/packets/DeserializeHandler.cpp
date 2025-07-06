#include "packets/DeserializeHandler.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "packets/PacketDatabase.h"

ctpl::thread_pool DeserializeHandler::curl_pool(5);

void DeserializeHandler::deserialize(const uint32_t in_pid, const std::vector<uint8_t>* data)
{
    if (data == nullptr)
    {
        return;
    }

    const PacketInfo header = static_cast<PacketInfo>((*data)[0] | (*data)[1] << 8);
    const packet_detail* pkt_detail = PacketDatabase::get(header);

    uint8_t start_data = 2;
    if (pkt_detail->size == -1)
    {
        start_data = 4;
    }
    pkt_data = std::span(data->data() + start_data, data->size() - start_data);
    pid = in_pid;

    if (pkt_data.size() > 0)
    {
        deserialize_internal(header);        
    }    
}

nlohmann::json DeserializeHandler::get_app_config()
{
    std::ifstream config_file("config.json");
    nlohmann::json config = nullptr;
    if (config_file)
    {
        config_file >> config;
    }
    return config;
}

void DeserializeHandler::debug_packet() const
{
    std::cout << "[";
    for (unsigned char i : pkt_data)
    {
        std::cout << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(i) << " ";
    }
    std::cout << "]" << std::dec << std::endl;
}

std::string DeserializeHandler::string_to_hex(const std::string& input)
{
    std::ostringstream oss;
    for (unsigned char c : input) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

void DeserializeHandler::send_request(const std::string& endpoint, nlohmann::json& in_data)
{
    nlohmann::json app_config = get_app_config();
    if (!app_config.contains("api"))
    {
        return;
    }

    if (!app_config["api"].contains("url") || !app_config["api"].contains("key"))
    {
        return;
    }
    
    in_data["server_id"] = 0;
    if (app_config.contains("server_id"))
    {
        in_data["server_id"] = app_config["server_id"];
    }
    
    curl_pool.push([=](int)
    {
        CURL* curl = curl_easy_init();
        if (curl)
        {
            CURLcode res;
            const std::string url = std::format("{}/{}", std::string(app_config["api"]["url"]), endpoint);
            std::string response_string;
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, std::format("X-API-KEY: {}", std::string(app_config["api"]["key"])).c_str());

            std::string data = in_data.dump();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(data.size()));
            curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data.c_str());
            
            // Set write function to capture response
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DeserializeHandler::write_callback);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cerr << "Error in curl_easy_perform(): " << curl_easy_strerror(res) << '\n';
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
    });
}

size_t DeserializeHandler::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto& response = *static_cast<std::string*>(userdata);
    response.append(ptr, size * nmemb);
    return size * nmemb;
}