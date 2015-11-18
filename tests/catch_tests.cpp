#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "catch_tests.h"

std::string hexStr(const std::string& data)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<data.size();++i)
        ss << std::setw(2) << std::setfill('0') << ((int)data[i] & 0x0ff);
    return ss.str();
}
