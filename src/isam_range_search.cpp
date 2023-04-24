//
// Created by juandiego on 4/24/23.
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

    ISAM<true, char[5], Record, std::function<char *(Record &)>, std::function<bool(char[5], char[5])>> isam(
            "../database/data.dat", index, greater
    );

    char lower_bound [5];
    char upper_bound [5];

    std::cout << "Lower bound: ";
    func::read_buffer(lower_bound, 5);

    std::cout << "Upper bound: ";
    func::read_buffer(upper_bound, 5);

    auto records = isam.range_search(lower_bound, upper_bound);
    for (Record& record : records) {
        std::cout << record.to_string() << std::endl;
    }

    return EXIT_SUCCESS;
}
