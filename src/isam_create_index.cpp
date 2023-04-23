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
    isam.build_static_tree("../database/sorted_data.dat");

    return EXIT_SUCCESS;
}
