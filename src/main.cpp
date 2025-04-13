#include "public/Sniffer.h"

int main() {
    Sniffer sniffer;
    sniffer.start_capture("35.199.111.15");
    return 0;
}
