#include <vector>

#include "public/Sniffer.h"

int main() {
    Sniffer sniffer;
    std::vector<std::string> ip_list =
    {
        // login server
        "35.199.111.15",
        // map server
        "35.198.41.33", // prontera - map
        "34.95.145.188", // prontera -inn
    };
    sniffer.start_capture(ip_list);
    return 0;
}
