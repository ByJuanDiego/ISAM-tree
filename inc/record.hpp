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

void init(Record &record) {
    std::cout << "----------- new record -----------" << std::endl;
    std::cout << "code: ";
    func::read_buffer(record.code, 5);
    std::cout << "name: ";
    func::read_buffer(record.name, 20);
    std::cout << "cycle: ";
    std::cin >> record.cycle;
}

#endif //RECORD_HPP
