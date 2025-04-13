#include <string>
#include <pcap/pcap.h>

class Sniffer {
public:
    Sniffer();
    ~Sniffer();

    void start_capture();
    void stop_capture();
    void process_packet(const struct pcap_pkthdr *header, const u_char *packet);

private:
    pcap_if_t* get_capture_device();
    std::string select_capture_device(const pcap_if_t* all_devs);
    
    pcap_t *handle;
    pcap_if_t* capture_device;
};
