#pragma once
#include <vector>
#include <string>
#include <sstream>

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