#include "../public/Sniffer.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../public/packets/PacketDatabase.h"

Sniffer::Sniffer() {}

Sniffer::~Sniffer()
{
    stop_capture();
}

void Sniffer::start_capture(const std::vector<std::string>& ip_capture)
{
    // Set device~
    capture_device = get_capture_device();
    if (!capture_device)
    {
        std::cerr << "Error: No capture device found\n";
        return;
    }

    char err_buf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(capture_device->name, 65536, 1, 1000, err_buf);
    if (!handle) {
        std::cerr << "Failed to open device: " << err_buf << '\n';
        return;
    }

    std::cout << "Started packet capture..." << '\n';
    std::string filter_exp;
    for (size_t i = 0; i < ip_capture.size(); ++i)
    {
        filter_exp += "src host "+ ip_capture[i];
        if (i != ip_capture.size() - 1)
            filter_exp += " or ";
    }
    
    if (!apply_filter(filter_exp)) {
        std::cerr << "Failed to set filter: " << filter_exp << '\n';
        return;
    }

    pcap_loop(handle, 0, packet_handler, nullptr);
    return;
}

bool Sniffer::apply_filter(const std::string& filterExpression) const
{
    struct bpf_program fp;
    if (pcap_compile(handle, &fp, filterExpression.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "Error compiling filter: " << pcap_geterr(handle) << '\n';
        return false;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "Error setting filter: " << pcap_geterr(handle) << '\n';
        return false;
    }

    return true;
}

void Sniffer::stop_capture()
{
    if (capture_device)
    {
        pcap_freealldevs(capture_device);
        capture_device = nullptr;
    }
}

pcap_if_t* Sniffer::get_capture_device()
{
    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_if_t* all_devs;
    if (pcap_findalldevs(&all_devs, err_buf) == -1)
    {
        std::cerr << "Error finding devices: %s" << err_buf << '\n';
        return nullptr;
    }

    nlohmann::json config;
    std::string device_name;
    std::ifstream config_file("config.json");
    if (!config_file)
    {
        std::cout << "No config.json found. Select net devices:" << '\n';
        device_name = select_capture_device(all_devs);

        config["device_id"] = device_name;
        std::ofstream out_config_file("config.json");
        out_config_file << config.dump(4);
    }
    else
    {
        config_file >> config;
        device_name = config.value("device_id", "NONE");
    }

    for (pcap_if_t* d = all_devs; d != nullptr; d = d->next)
    {
        if (d->name && d->name == device_name)
        {
            std::cout << "Selected Net Device for capture: " << d->description << std::endl;
            return d;
        }
    }
        
    return nullptr;
}

std::string Sniffer::select_capture_device(const pcap_if_t* all_devs)
{
    int i = 0;
    for (const pcap_if_t* d = all_devs; d != nullptr; d = d->next)
    {
        std::cout << i++ << " : [" << d->name << "] ";
        if (d->description)
            std::cout << "- " << d->description;
        std::cout << '\n';
    }

    int device_id;
    std::cout << "Select device id: " << std::endl;
    do
    {
        if (std::cin >> device_id)
        {
            i = 0;
            for (const pcap_if_t* d = all_devs; d != nullptr; d = d->next)
            {
                if (i == device_id)
                {
                    return d->name;
                }
                i++;
            }
        }
        else
        {            
            std::cout << "Invalid input. Please enter a number: ";
        }
    } while (true);
    
}

void Sniffer::packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data)
{
    const u_char* ip_header = pkt_data + Sniffer::ETHERNET_HEADER_LEN;
    int ip_header_len = (*ip_header & 0x0F) * 4;
    u_char protocol = *(ip_header + 9);

    const u_char* transport_header = ip_header + ip_header_len;
    int transport_header_len = 0;

    if (protocol == Sniffer::TCP)
    {
        transport_header_len = ((*(transport_header + 12)) >> 4) * 4;
    }
    else if (protocol == Sniffer::UDP)
    {
        transport_header_len = 8;
    }
    const u_char* payload = transport_header + transport_header_len;

    int total_header_size = ETHERNET_HEADER_LEN + ip_header_len + transport_header_len;
    unsigned int payload_len = header->len - total_header_size;
    
    deserialization(payload, payload_len);
}

void Sniffer::deserialization(const u_char* payload, const unsigned int payload_len)
{
    // ----------------- deserialization ----------------
    uint16_t packet_id = (payload[1] << 8) | payload[0];
    std::cout << "["<< packet_id <<"]";
    const packet_detail* pkt_dt = PacketDatabase::get(packet_id);

    std::cout << "Packet captured: ";
    if (!pkt_dt)
    {
        std::cout << "[" << std::setw(3) << std::setfill('0') << payload_len << "]";
        std::cout << " data = ";
        for (unsigned int i = 0; i < payload_len; ++i)
        {
            std::cout << std::uppercase
                      << std::hex
                      << std::setw(2)
                      << std::setfill('0')
                      << static_cast<int>(payload[i]) << " ";
        }
        std::cout << std::dec << std::endl;
        return;
    }

    if (pkt_dt->size > 0) // variable size
    {        
        uint16_t packet_size = (payload[3] << 8) | payload[2];
        const u_char* packet_data = payload + 4;
    }
    else // fix size
    {
        const u_char* packet_data = payload + 2;
    }
    std::cout << std::dec << std::endl;
    
}
