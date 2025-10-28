#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "gameplay/exp_calculator/ExpCalculator.h"
#include "pixelbot/PixelBot.h"
#include "public/Sniffer.h"

namespace
{
    void capture(bool save = false);
    void reproduce(const std::string& filename);
}

int main(int argc, char* argv[])
{
    std::cout << "RO Sniffer" << std::endl;
    if (argc < 2)
    {
        capture();
    }
    
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--save")
        {
            capture(true);
        }
        else if (arg == "--reproduce")
        {
            if (argv[i + 1] == nullptr)
            {
                std::cerr << "Usage: --reproduce <filename>" << std::endl;
                return 1;
            }
            reproduce(argv[i+1]);
            break;
        }
        else if (arg == "--exp")
        {
            std::thread exp_show(ExpCalculator::show_exp);
            capture();
            exp_show.join();
        }
        else if (arg == "--shops")
        {
            std::thread shop_capture(PixelBot::run);
            capture();
            shop_capture.join();
        }
        else
        {
            std::cerr << "Usage: " << argv[0] << " [--save] [--reproduce <filename>]" << std::endl;
            return 1;
        }
    }
    return 0;
}

namespace
{
    void capture(bool save)
    {
        Sniffer::get()->start_capture(save);
    }

    void reproduce(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file)
        {
            std::cerr << "Can't open file: " << filename << std::endl;
            return;
        }

        std::string lineStr;
        int lineNumber = 1;

        while (std::getline(file, lineStr))
        {
            std::istringstream iss(lineStr);
            std::string byteStr;
            std::vector<uint8_t> lineBytes;

            while (iss >> byteStr)
            {
                try
                {
                    uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
                    lineBytes.push_back(byte);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error convert '" << byteStr << "' in line " << lineNumber << ": " << e.what() << std::endl;
                }
            }

            // To test
            u_char* char_ptr = reinterpret_cast<u_char*>(lineBytes.data());
            Sniffer::get()->self_test(char_ptr, lineBytes.size());
            //std::cout << "Packet " << lineNumber << " ------------- " << std::endl;

            ++lineNumber;
        }
    }    
}