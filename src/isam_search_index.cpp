//
// Created by juandiego on 4/22/23.
//

#include "../inc/ISAM.hpp"
#include "../inc/record.hpp"

int main() {
    std::function<char *(Record &)> index = [](Record &record) -> char * {
        return record.code;
    };

    std::function<bool(char[5], char[5])> greater = [](char a[5], char b[5]) -> bool {
        return std::string(a) > std::string(b);
    };

    ISAM<true, char[5], Record, std::function<char *(Record &)>, std::function<bool(char[5], char[5])>> isam("../database/data.dat", index, greater);

    std::ifstream data_file("../database/sorted_data.dat", std::ios::binary);
    Record record {};

    while (data_file.read((char *) &record, SIZE(Record))) {
        std::cout << isam.search(record.code)[0].to_string() << std::endl;
    }
    data_file.close();

    return EXIT_SUCCESS;
}
