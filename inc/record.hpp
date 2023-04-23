//
// Created by juandiego on 4/19/23.
//

#ifndef RECORD_HPP
#define RECORD_HPP

#include <cmath>
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

void read_from_console(char buffer[], int size) {
    std::string temp;
    std::getline(std::cin >> std::ws, temp, '\n');
    std::cin.clear();

    int sz = std::min((int) temp.length(), size);
    for (int i = 0; i < sz; ++i) {
        buffer[i] = temp[i];
    }

    buffer[size - 1] = '\0';
}

void init(Record &record) {
    std::cout << "----------- new record -----------" << std::endl;
    std::cout << "code: ";
    read_from_console(record.code, 5);
    std::cout << "name: ";
    read_from_console(record.name, 20);
    std::cout << "cycle: ";
    std::cin >> record.cycle;
}

#endif //RECORD_HPP
