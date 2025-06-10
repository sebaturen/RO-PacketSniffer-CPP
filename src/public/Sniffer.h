#pragma once

#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>
#include <pcap/pcap.h>

namespace SnifferSpace
{
    extern std::vector<std::string> ro_latam_ip_list;
}

class Sniffer {
public:
    Sniffer();
    ~Sniffer();

    void start_capture(bool save = false);
    void stop_capture();
    void self_test(const u_char* payload, const unsigned int payload_len );

private:
    pcap_if_t* get_capture_device();
    std::vector<std::string> get_capture_ips() const;
    static std::string select_capture_device(const pcap_if_t* all_devs);
    static void save_payload(const u_char* payload, unsigned int payload_len);
    static void packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data);
    static void processIncomingData(const uint16_t dst_port, const u_char* payload, const unsigned int payload_len);
    static size_t processHttpPacket(const std::vector<uint8_t>& buffer, bool& valid);
    static void update_pip_port();

    static void log(const std::string& msg);
    static std::string hexStr(uint16_t val);
    static bool isAscii(uint16_t byte);

    static void debug_payload(const u_char* payload, unsigned int payload_len);
    
    bool apply_filter(const std::string& filterExpression) const;
    
    pcap_t *handle;
    nlohmann::json config;
    pcap_if_t* capture_device = nullptr;
    inline static u_int64 m_packet_count = 0;
    static std::unordered_map<uint16_t, uint32_t> port_pid_map; // < Port , PID >
    static std::unordered_map<uint16_t, std::vector<u_char>> m_buffer_map;
    static std::vector<std::thread> threads;
    inline static bool bSaveCapture = false;
    
    inline static bool bDebugMode = false;
    inline static uint16_t lastKnowHeader = 0;

};
