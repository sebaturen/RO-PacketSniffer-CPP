#include "../../public/packets/DeserializeHandler.h"

#include <iomanip>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "../../public/packets/PacketDatabase.h"

nlohmann::json DeserializeHandler::app_config;
ctpl::thread_pool DeserializeHandler::curl_pool(10);

void DeserializeHandler::set_app_config(const nlohmann::json& in_app_config)
{
    app_config = in_app_config;
}

void DeserializeHandler::deserialize(const std::vector<uint8_t>* data)
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
    deserialize_internal(header);
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

void DeserializeHandler::send_request(const std::string& endpoint, const std::string& data)
{
    if (!app_config.contains("api"))
    {
        std::cout << "[INFO] API not enabled. Skipping request." << std::endl;
        return;
    }

    if (!app_config["api"].contains("url") || !app_config["api"].contains("key"))
    {
        std::cout << "[INFO] API URL or key not set. Skipping request." << std::endl;
        return;
    }

    curl_pool.push([=](int)
    {
        CURL* curl = curl_easy_init();
        if (curl)
        {
            CURLcode res;
            const std::string url = std::format("{}/{}", std::string(app_config["api"]["url"]), endpoint);
            //url += "/"+ endpoint;
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data.size());
            curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data.c_str());

            // DEBUG MODE
            /*curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
                std::cout << std::string(ptr, size * nmemb);
                return size * nmemb;
            });*/

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
