#include "../public/Sniffer.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../public/packets/DeserializeHandler.h"
#include "../public/packets/PacketDatabase.h"

std::vector<u_char> Sniffer::m_buffer;

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

    // validate zero payload
    if (payload_len > 0) {
        std::vector<unsigned char> zeros_compare(payload_len, 0);
        if (std::memcmp(payload, zeros_compare.data(), payload_len) == 0)
        {
            return;
        }
    } else {
        return;
    }

    // DEBUG MODE
    //debug_payload(payload, payload_len);
    // END DEBUG MODE
    
    deserialization(payload, payload_len);
}

void Sniffer::deserialization(const u_char* payload, const unsigned int payload_len)
{
    // Insert the new payload into the buffer
    m_buffer.insert(m_buffer.end(), payload, payload + payload_len);

    // Obtain the list of valid packet IDs
    const auto& valid_ids = PacketDatabase::get_all_ids();

    size_t offset = 0;
    while (offset < m_buffer.size()) // Ensure we do not go out of bounds
    {
        size_t remaining = m_buffer.size() - offset;
        
        if (remaining < 2) 
            break; // Can't process a packet header if there aren't at least 2 bytes

        // Extract the packet ID from the first two bytes
        uint16_t packet_id = (m_buffer[offset + 1] << 8) | m_buffer[offset];
        const packet_detail* pkt_dt = PacketDatabase::get(packet_id);

        if (!pkt_dt || pkt_dt->size == 0) // find sub pack in unknown packet
        {            
            // DEBUG MODE
            std::cout << "Resyncing... -> ["<< std::hex << packet_id << "]" << std::endl;
            debug_payload(m_buffer.data(), m_buffer.size());
            // END DEBUG MODE
            bool resynced = false;

            // Try to find a valid packet ID by sliding through the buffer
            for (size_t i = 1; i < remaining - 1; ++i)
            {                
                uint16_t potential_id = (m_buffer[offset + i + 1] << 8) | m_buffer[offset + i];
                if (valid_ids.contains(potential_id))
                {
                    offset += i; // Adjust offset to resync
                    resynced = true;
                    break;
                }
            }

            // If we couldn't resync, break out of the loop
            if (!resynced)
                break;

            continue;
        }
        
        size_t total_size = 0;

        // Determine total size based on whether the packet size is fixed or variable
        if (pkt_dt->size == -1)
        {
            if (remaining < 4)
                break;

            total_size = (m_buffer[offset + 3] << 8) | m_buffer[offset + 2];
        }
        else // Fixed size
        {
            total_size = pkt_dt->size;
        }

        if (total_size > remaining) // packet uncompleted
            break;

        // Call the packet handler to deserialize the data
        if (pkt_dt->handler)
        {
            pkt_dt->handler->deserialize(&m_buffer[offset], total_size);
        }
        
        // Move the offset forward by the total size of the processed packet
        offset += total_size;
    }

    // Erase the processed data from the buffer
    if (offset > 0)
    {
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + offset);
    }
}

void Sniffer::debug_payload(const u_char* payload, unsigned int payload_len)
{
    std::cout << "[";
    for (unsigned int i = 0; i < payload_len; ++i)
    {
        std::cout << std::uppercase
                  << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(payload[i]) << " ";
    }
    std::cout << "]" << std::dec << std::endl;
}
