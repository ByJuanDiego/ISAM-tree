//
// Created by juandiego on 4/19/23.
//

#ifndef RECORD_HPP
#define RECORD_HPP

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

#include "utils.hpp"

#include <string>
#include <sstream>

struct MovieRecord {
    int dataId{};
    char contentType[16]{'\0'};
    char title[256]{'\0'};
    short length{};
    short releaseYear{};
    short endYear{};
    int votes{};
    float rating{};
    int gross{};
    char certificate[16]{'\0'};
    char description[512]{'\0'};
    bool removed{};

    std::string to_string() {
        std::stringstream ss;
        ss << "("
           << dataId << ", " << contentType << ", " << title << ", " << length << ", " << releaseYear << ", "
           << endYear << ", " << votes << ", " << rating << ", " << gross << ", " << certificate
           << ", " << std::boolalpha << removed << ")";
        return ss.str();
    }
};

#endif //RECORD_HPP
