#include "../public/Sniffer.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

Sniffer::Sniffer() {}

Sniffer::~Sniffer()
{
    stop_capture();
}

void Sniffer::start_capture(const std::string& ip_capture)
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
    std::string filter_exp = "dst host " + ip_capture;
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
    std::cout << "Packet captured: length = " << header->len << std::endl;
    std::cout << "Packet data: ";
    for (unsigned int i = 0; i < header->len; ++i)
    {
        std::cout << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(pkt_data[i]) << " ";
    }
    
    std::cout << std::dec << std::endl;
}

void Sniffer::process_packet(const struct pcap_pkthdr* header, const u_char* packet)
{
}