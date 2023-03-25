#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

std::vector<std::string> split(const std::string &str, const char delim)
{
    std::vector<std::string> strings;
    std::istringstream stream(str);
    std::string s;
    while (std::getline(stream, s, delim)) {
        strings.push_back(s);
    }
    return strings;
}

uint32_t getContainerInstance()
{
    uint32_t result = 0;
    std::string hostname(getenv("HOSTNAME"));

    // Expect stateful set hostnames to end in -<instance>
    size_t pos = hostname.find_last_of('-');
    if (pos != std::string::npos) {
        std::string instance = hostname.substr(pos+1);
        try {
            result = static_cast<uint32_t>(std::stoul(instance,nullptr,10));
        }
        catch (...) {
            // Contain exception
        }
    }
    return result;
}