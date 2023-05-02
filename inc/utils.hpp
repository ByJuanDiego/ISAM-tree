//
// Created by juandiego on 4/23/23.
//

#ifndef ISAM_UTILS_HPP
#define ISAM_UTILS_HPP

#include <cmath>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <functional>

namespace func {

    struct clock {
        template<typename Function, typename... Params>
        void operator()(const Function &fun, const std::string &function_name, const Params &...params) {
            std::cout << "Executing function " << function_name << "..." << std::endl;
            const auto start = std::chrono::steady_clock::now();
            fun(params...);
            const auto end = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Executed function " << function_name << " in " << duration << " microseconds." << std::endl;
        }
    };

    template<typename T>
    void copy(T &a, const T &b) {
        std::memcpy((char *) &a, (char *) &b, sizeof(T));
    }

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

        for (int i = 0; i < size; ++i) {
            buffer[i] = (i < temp.size()) ? temp[i] : '\0';
        }

        buffer[size - 1] = '\0';
    }

}


#endif //ISAM_UTILS_HPP
