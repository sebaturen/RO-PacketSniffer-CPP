#pragma once

#include <string>
#include <vector>
#include <pcap/pcap.h>

class Sniffer {
public:
    Sniffer();
    ~Sniffer();

    void start_capture(const std::vector<std::string>& ip_capture);
    void stop_capture();

private:
    static pcap_if_t* get_capture_device();
    static std::string select_capture_device(const pcap_if_t* all_devs);
    static void packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data);
    static void deserialization(const u_char* payload, const unsigned int payload_len);
    
    bool apply_filter(const std::string& filterExpression) const;
    
    pcap_t *handle;
    pcap_if_t* capture_device;

    // Static constants
    constexpr static int ETHERNET_HEADER_LEN = 14;
    constexpr static int TCP = 6;
    constexpr static int UDP = 17;
};