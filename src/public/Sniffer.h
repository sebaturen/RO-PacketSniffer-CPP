#pragma once

#include <string>
#include <vector>
#include <pcap/pcap.h>

#include "packets/PacketDatabase.h"

class Sniffer {
public:
    Sniffer() =default;
    ~Sniffer();

    void start_capture(const std::vector<std::string>& ip_capture);
    void stop_capture();
    void self_test(const u_char* payload, const unsigned int payload_len );

private:
    static pcap_if_t* get_capture_device();
    static std::string select_capture_device(const pcap_if_t* all_devs);
    static void save_payload(const u_char* payload, unsigned int payload_len);
    static void packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data);
    static void processIncomingData(const u_char* payload, const unsigned int payload_len);

    static void log(const std::string& msg);
    static std::string hexStr(uint16_t val);
    static bool isAscii(uint16_t byte);

    static void debug_payload(const u_char* payload, unsigned int payload_len);
    
    bool apply_filter(const std::string& filterExpression) const;
    
    pcap_t *handle;
    pcap_if_t* capture_device;
    static std::vector<u_char> m_buffer;
    inline static bool bDebugMode = true;
    inline static uint16_t lastKnowHeader = 0;

};
