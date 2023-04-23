#include <iostream>
#include <random>
#include <iomanip>

#include "../inc/ISAM.hpp"
#include "../inc/record.hpp"

int main() {
    std::cout << N<Record> << " " << M<char[5]> << std::endl;
    std::ofstream sorted_file("../database/sorted_data.dat", std::ios::app);

    for (int i = 0, j = 4; i < 2187; ++i, j += 4) {
        std::string name;
        std::string code;

        Record record{};
        code = std::to_string(j);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << code;

        std::memcpy(record.code, ss.str().c_str(), 5);
        record.code[4] = '\0';

        std::random_device rd;
        std::uniform_int_distribution<int> letters(65, 90);
        name = std::string(20, ' ');
        for (int k = 0; k < 19; ++k) {
            name[k] = (char) letters(rd);
        }
        std::memcpy(record.name, name.c_str(), 20);
        record.name[19] = '\0';
        name.clear();

        std::uniform_int_distribution<int> cycles(1, 10);
        record.cycle = cycles(rd);

        sorted_file.write((char *) &record, SIZE(Record));
    }
    sorted_file.close();

    return EXIT_SUCCESS;
}
