#include "Sniffer.h"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>
#include <iphlpapi.h>

#include "gameplay/exp_calculator/ExpCalculator.h"
#include "packets/DeserializeHandler.h"
#include "packets/PacketDatabase.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace SnifferSpace
{
    constexpr size_t MAX_BUFFER_SIZE = 1024 * 1024 * 1024; // 1GB
    constexpr size_t MAX_HTTP_CHUNK_SIZE = 64 * 1024; // 64KB
    constexpr static int ETHERNET_HEADER_LEN = 14;
    constexpr static int TCP = 6;
    constexpr static int UDP = 17;
}

std::unordered_map<uint16_t, uint32_t> Sniffer::port_pid_map;
std::unordered_map<uint16_t, std::vector<u_char>> Sniffer::m_buffer_map;
std::vector<std::thread> Sniffer::threads;

Sniffer::Sniffer()
{    
    std::ifstream config_file("config.json");
    if (!config_file)
    {
        std::ofstream out_config_file("config.json");
        config = nlohmann::json::object();
        out_config_file << config.dump(4);
    }
    else
    {
        config_file >> config;
    }
    
    if (!config.contains("api"))
    {
        std::cout << "[INFO] API not enabled. Skipping requests." << std::endl;
    }
    if (!config["api"].contains("url") || !config["api"].contains("key"))
    {
        std::cout << "[INFO] API URL or key not set. Skipping requests." << std::endl;
    }
}

Sniffer::~Sniffer()
{
    stop_capture();
}

void Sniffer::start_capture(bool save)
{
    bSaveCapture = save;
    // Set device~
    capture_device = get_capture_device();
    if (!capture_device)
    {
        std::cerr << "Error: No capture device found\n";
        return;
    }
    std::vector<std::string> ip_capture = get_capture_ips();
    if (ip_capture.empty())
    {
        std::cerr << "No target IPs configured in config.json\n";
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

    bCaptureStarted = true;
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
    
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Sniffer::self_test(const u_char* payload, const unsigned int payload_len )
{    
    processIncomingData(0, payload, payload_len);
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
    
    std::string device_name;    
    if (!config.contains("device_id"))
    {        
        std::cout << "Select net devices:" << '\n';
        device_name = select_capture_device(all_devs);

        config["device_id"] = device_name;
        std::ofstream out_config_file("config.json");
        out_config_file << config.dump(4);
    }
    else
    {
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

std::vector<std::string> Sniffer::get_capture_ips() const
{
    if (!config.contains("capture_ips"))
    {
        std::cerr << "No target IPs configured in config.json" << std::endl;
        return {};
    }

    return config["capture_ips"].get<std::vector<std::string>>();
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
        outFile << std::hex << std::uppercase
                << std::setw(2) << std::setfill('0')
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
    uint16_t dst_port = (transport_header[2] << 8) | transport_header[3];
    
    int transport_header_len = ((*(transport_header + 12)) >> 4) * 4;
    const u_char* payload = transport_header + transport_header_len;

    int total_header_size = SnifferSpace::ETHERNET_HEADER_LEN + ip_header_len + transport_header_len;
    unsigned int payload_len = header->len - total_header_size;
    
    m_packet_count++;
    if (bDebugMode)
    {        
        std::cout << "Coming: ["<< dst_port <<"] ";
        debug_payload(payload, payload_len);        
    }
    
    //std::cout << "\rPacket Count: " << m_packet_count << std::flush;
    if (bSaveCapture)
    {
        save_payload(payload, payload_len);
    }
    
    processIncomingData(dst_port, payload, payload_len);
}

void Sniffer::processIncomingData(const uint16_t dst_port, const u_char* payload, const unsigned int payload_len)
{
    std::vector<u_char>& m_buffer = m_buffer_map[dst_port];
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
        const packet_detail* detail = PacketDatabase::get(static_cast<PacketInfo>(header));

        if (!detail)
        {
            std::string msg = "[INFO] Unknown packet. Header: " + hexStr(header);
            if (lastKnowHeader != 0)
            {
                msg += " lastKnowHeader: " + hexStr(lastKnowHeader);
            }
            log(msg);
            debug_payload(m_buffer);
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
            log("[INFO] Valid packet. Header: ["+ detail->desc +"] " + hexStr(header) + " Size: " + std::to_string(packetSize));
            if (detail->alert)
            {
                log("[WARN] Alert packet. Header: " + hexStr(header) + " Size: " + std::to_string(packetSize));
                debug_payload(m_buffer.data(), m_buffer.size());
            }
            lastKnowHeader = header;
            log("[INFO] Deserializing packet. Header: " + hexStr(header) + " Size: " + std::to_string(packetSize));
            std::vector<uint8_t> packetCopy{ m_buffer.begin(), m_buffer.begin() + packetSize };
            if (detail->handler)
            {
                uint32_t process_id = 0;
                if (dst_port != 0)
                {
                    auto pid_it = port_pid_map.find(dst_port);
                    if (pid_it == port_pid_map.end())
                    {
                        update_pip_port();
                        pid_it = port_pid_map.find(dst_port);
                    }
                    process_id = pid_it->second;                    
                }
                
                threads.emplace_back([detail = detail, packet = packetCopy, pid = process_id]() {
                    std::unique_ptr<DeserializeHandler> inHandler = detail->handler();
                    inHandler->deserialize(pid, &packet);
                });
            }

            // the minimum size packet detected is 6bytes, so packates have len < 6, are filling with 0x00
            // to prevent errors and process un-existing packets, we can skip this content
            if (payload_len == 6 && packetSize < m_buffer.size())
            {
                bool bCanSkip = true;
                uint8_t zero_bytes = 0;
                for (size_t i = packetSize; i < m_buffer.size(); i++)
                {
                    if (m_buffer[i] != 0x00)
                    {
                        bCanSkip = false;
                        break;
                    }
                    zero_bytes++;
                }
                if (bCanSkip)
                {
                    packetSize += zero_bytes;
                }
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

            if (totalSize >= SnifferSpace::MAX_HTTP_CHUNK_SIZE)
            {
                log("[WARN][HTTP] Buffer size exceeded, clearing buffer.");
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

void Sniffer::update_pip_port()
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    PMIB_TCPTABLE_OWNER_PID pTcpTable = static_cast<MIB_TCPTABLE_OWNER_PID*>(malloc(sizeof(MIB_TCPTABLE_OWNER_PID)));
    dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);

    // Check size
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(pTcpTable);
        pTcpTable = static_cast<MIB_TCPTABLE_OWNER_PID*>(malloc(dwSize));
    }

    // Get TCP table
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == NO_ERROR)
    {
        for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++)
        {
            MIB_TCPROW_OWNER_PID row = pTcpTable->table[i];
            
            const auto pid = row.dwOwningPid;
            auto pid_port = ntohs(static_cast<u_short>(row.dwLocalPort));

            port_pid_map.insert_or_assign(pid_port, pid);
        }
    }

    if (pTcpTable) {
        free(pTcpTable);
    }
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

void Sniffer::debug_payload(const std::vector<u_char>& payload)
{
    std::cout << "[";
    for (const u_char c : payload)
    {
        std::cout << std::uppercase
                    << std::hex
                    << std::setw(2)
                    << std::setfill('0')
                    << static_cast<int>(c) << " ";
    }
    std::cout << "]" << std::dec << std::endl;
}

std::string Sniffer::hexStr(uint16_t val)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(4) << std::setfill('0') << val;
    return oss.str();
}
