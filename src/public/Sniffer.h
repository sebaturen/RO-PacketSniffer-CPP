#include <string>
#include <pcap/pcap.h>

class Sniffer {
public:
    Sniffer();
    ~Sniffer();

    void start_capture(const std::string& ip_capture);
    void stop_capture();

private:
    static pcap_if_t* get_capture_device();
    static std::string select_capture_device(const pcap_if_t* all_devs);
    static void packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data);
    
    bool apply_filter(const std::string& filterExpression) const;
    void process_packet(const struct pcap_pkthdr *header, const u_char *packet);
    
    pcap_t *handle;
    pcap_if_t* capture_device;
};