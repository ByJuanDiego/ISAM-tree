#include <iostream>
#include "ISAM.hpp"
#include "record.hpp"

int main() {
    std::function<char *(Record &)> index = [&](Record &record) -> char * {
        return record.code;
    };

    ISAM<true, char[5], Record, TYPE(index)> isam;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
