//
// Created by juandiego on 4/19/23.
//

#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>
#include <sstream>
#include <fstream>

struct Record {
    char name[20];
    char code[5];
    int cycle;

    std::string to_string() {
        std::stringstream ss;
        ss << "(" << code << ", " << name << ", " << cycle << ")";
        return ss.str();
    }
};

#endif //RECORD_HPP
