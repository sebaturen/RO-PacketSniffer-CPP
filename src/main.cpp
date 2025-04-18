#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "public/Sniffer.h"

void test_pack_file(const std::string& filename);

int main() {
    //Sniffer sniffer;
    std::vector<std::string> ip_list =
    {
        // login server
        "35.199.111.15",
        // map server
        "35.198.41.33", // prontera - map
        "34.95.145.188", // prontera -inn
    };
    //sniffer.start_capture(ip_list);
    test_pack_file("pre_event_log_packets.txt");
    return 0;
}

void test_pack_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Can't open file: " << filename << std::endl;
        return;
    }

    Sniffer sniffer;
    std::string lineStr;
    int lineNumber = 1;

    while (std::getline(file, lineStr)) {
        std::istringstream iss(lineStr);
        std::string byteStr;
        std::vector<uint8_t> lineBytes;

        while (iss >> byteStr) {
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
                lineBytes.push_back(byte);
            } catch (const std::exception& e) {
                std::cerr << "Error convert '" << byteStr << "' in line " << lineNumber << ": " << e.what() << std::endl;
            }
        }

        // To test
        u_char* char_ptr = reinterpret_cast<u_char*>(lineBytes.data());
        sniffer.self_test(char_ptr, lineBytes.size());
        std::cout << "Packet " << lineNumber << " ------------- " << std::endl;

        ++lineNumber;
    }
}