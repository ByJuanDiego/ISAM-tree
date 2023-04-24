//
// Created by juandiego on 4/23/23.
//

#ifndef ISAM_UTILS_HPP
#define ISAM_UTILS_HPP

#include <cstring>

namespace func {

    template<typename T>
    void copy(T &a, T &b) {
        std::memcpy((char *) &a, (char *) &b, sizeof(T));
    }

    template<typename T>
    void copy(T &a, char *&b) {
        std::memcpy((char *) &a, b, sizeof(T));
    }

    void read_buffer(char buffer[], int size) {
        std::string temp;
        std::getline(std::cin >> std::ws, temp, '\n');
        std::cin.clear();

        int sz = std::min((int) temp.length(), size);
        for (int i = 0; i < sz; ++i) {
            buffer[i] = temp[i];
        }

        buffer[size - 1] = '\0';
    }

}


#endif //ISAM_UTILS_HPP
