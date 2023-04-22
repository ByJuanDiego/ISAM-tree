#include <iostream>
#include "ISAM.hpp"
#include "record.hpp"

int main() {
    std::function<char *(Record &)> index = [](Record &record) -> char * {
        return record.code;
    };

    ISAM<true, char[5], Record, TYPE(index)> isam("./database/data.dat", index);
    std::cout << N<Record> << " " << M<char[5]> << std::endl;
    return EXIT_SUCCESS;
}
