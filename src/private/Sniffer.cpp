#include "../public/Sniffer.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

#include "../public/packets/DeserializeHandler.h"
#include "../public/packets/PacketDatabase.h"

namespace SnifferSpace
{
    constexpr size_t MAX_BUFFER_SIZE = 1024 * 1024 * 1024; // 1GB
    constexpr static int ETHERNET_HEADER_LEN = 14;
    constexpr static int TCP = 6;
    constexpr static int UDP = 17;
}

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
    std::string filter_exp = "(";
    for (size_t i = 0; i < ip_capture.size(); ++i)
    {
        filter_exp += "src host "+ ip_capture[i];
        if (i != ip_capture.size() - 1)
            filter_exp += " or ";
    }
    filter_exp += ") and not (port 80 or port 443)";
    
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

void Sniffer::self_test(const u_char* payload, const unsigned int payload_len )
{    
    processIncomingData(payload, payload_len);
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

void Sniffer::save_payload(const u_char* payload, unsigned int payload_len)
{
    std::ofstream outFile("log_packets.txt", std::ios::app);

    for (size_t i = 0; i < payload_len; ++i) {
        outFile << std::hex << std::uppercase   // salida en mayúscula (opcional)
                << std::setw(2) << std::setfill('0') // rellenar con 0 si es necesario
                << static_cast<int>(payload[i]) << " ";
    }
    outFile << std::endl;
    outFile.close();
}

void Sniffer::packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data)
{
    const u_char* ip_header = pkt_data + SnifferSpace::ETHERNET_HEADER_LEN;
    int ip_header_len = (*ip_header & 0x0F) * 4;
    u_char protocol = *(ip_header + 9);

    if (protocol != SnifferSpace::TCP)
    {        
        std::cout << "UDP Packet received" << std::hex << ip_header << std::endl;
        return;
    }
    const u_char* transport_header = ip_header + ip_header_len;
    
    int transport_header_len = ((*(transport_header + 12)) >> 4) * 4;
    const u_char* payload = transport_header + transport_header_len;

    int total_header_size = SnifferSpace::ETHERNET_HEADER_LEN + ip_header_len + transport_header_len;
    unsigned int payload_len = header->len - total_header_size;
    
    // DEBUG MODE
    std::cout << "Comming: ";
    debug_payload(payload, payload_len);
    //save_payload(payload, payload_len);
    // END DEBUG MODE

    processIncomingData(payload, payload_len);
}

void Sniffer::processIncomingData(const u_char* payload, const unsigned int payload_len)
{    
    m_buffer.insert(m_buffer.end(), payload, payload + payload_len);

    while (m_buffer.size() >= 2)
    {
        if (m_buffer.size() > SnifferSpace::MAX_BUFFER_SIZE)
        {
            log("[WARN] Buffer size exceeded, clearing buffer.");
            m_buffer.clear();
            return;
        }

        uint16_t header = static_cast<uint16_t>(m_buffer[0]) | (static_cast<uint16_t>(m_buffer[1]) << 8);
        const packet_detail* detail = PacketDatabase::get(header);

        if (!detail)
        {
            std::string msg = "[INFO] Unknown packet. Header: " + hexStr(header);
            if (lastKnowHeader != 0)
            {
                msg += " lastKnowHeader: " + hexStr(lastKnowHeader);
            }
            log(msg);
            m_buffer.erase(m_buffer.begin());
            continue;
        }

        size_t packetSize = 0;
        bool valid = false;
        switch (detail->type)
        {
        case PacketSizeType::FIXED: case PacketSizeType::FIXED_MIN:
            {
                packetSize = detail->size;
                valid = m_buffer.size() >= packetSize;
                break;                
            }

        case PacketSizeType::INDICATED_IN_PACKET:
            {
                if (m_buffer.size() >= 4)
                {
                    packetSize = static_cast<uint16_t>(m_buffer[2]) | (static_cast<uint16_t>(m_buffer[3]) << 8);
                    if (packetSize < 4 || packetSize > 10000)
                    {
                        log("[WARN] Invalid packet size (too small/to big) for header: " + hexStr(header));
                        m_buffer.erase(m_buffer.begin());
                        continue;                        
                    }
                    valid = m_buffer.size() >= packetSize;
                }
                break;                
            }
            
        case PacketSizeType::HTTP:
            {
                packetSize = processHttpPacket(m_buffer, valid);
                break;
            }
            
        case PacketSizeType::UNKNOWN:
            {
                log("[INFO] Unknown packet. Header: " + hexStr(header));
                m_buffer.erase(m_buffer.begin());
                continue;                
            }
        }

        if (valid)
        {
            if (detail->alert)
            {
                log("[WARN] Alert packet. Header: " + hexStr(header) + " Size: " + std::to_string(packetSize));
                debug_payload(m_buffer.data(), m_buffer.size());
            }
            lastKnowHeader = header;
            log("[INFO] Deserializing packet. Header: " + hexStr(header) + " Size: " + std::to_string(packetSize));
            std::span<const uint8_t> packetSpan{ m_buffer.data(), packetSize };
            if (detail->handler)
            {
                std::thread([handler = detail->handler, packet = std::vector<uint8_t>(packetSpan.begin(), packetSpan.end())]() {
                    handler->deserialize(packet);
                }).detach();
                
                //detail->handler->deserialize(packetSpan.data(), packetSize);
            }
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + packetSize);
        }
        else
        {
            break;
        }
    }
}

size_t Sniffer::processHttpPacket(const std::vector<uint8_t>& buffer, bool& valid)
{
    constexpr uint8_t delimiter[] = {0x0D, 0x0A, 0x0D, 0x0A};
    
    auto headerEndIt = std::search(buffer.begin(), buffer.end(), std::begin(delimiter), std::end(delimiter));
    if (headerEndIt == buffer.end())
    {
        valid = false;
        return 0;
    }

    size_t headerEnd = std::distance(buffer.begin(), headerEndIt) + 4;
    std::string headers(buffer.begin(), buffer.begin() + headerEnd);
    bool isChunked = headers.find("Transfer-Encoding: chunked") != std::string::npos;

    if (isChunked)
    {
        auto chunkEndIt = std::search(buffer.begin() + headerEnd, buffer.end(), std::begin(delimiter), std::end(delimiter));
        if (chunkEndIt != buffer.end())
        {
            size_t totalSize = std::distance(buffer.begin(), chunkEndIt) + 4;
            if (buffer.size() >= totalSize)
            {
                valid = true;
                return totalSize;
            }
        }

        valid = false;
        return 0;
    }

    auto pos = headers.find("Content-Length:");
    if (pos != std::string::npos)
    {
        size_t endLine = headers.find("\r\n", pos);
        std::string lenStr = headers.substr(pos + 15, endLine - pos - 15);
        size_t contentLength = static_cast<size_t>(std::stoi(lenStr));
        valid = buffer.size() >= headerEnd + contentLength;
        return headerEnd + contentLength;
    }

    // fallback: only header
    valid = true;
    return headerEnd;
}

void Sniffer::log(const std::string& msg)
{
    if (bDebugMode)
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&t), "%F %T") << " - " << msg << std::endl;
    }
}

bool Sniffer::isAscii(uint16_t byte)
{
    return std::isalpha(byte) || byte == '_' || byte == '-' || byte == '.';
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

std::string Sniffer::hexStr(uint16_t val)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << val;
    return oss.str();
}
