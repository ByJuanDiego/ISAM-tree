#include <iostream>

#include "../inc/ISAM.hpp"
#include "../inc/record.hpp"

int main() {

    std::ifstream sorted_file("../database/sorted_data.dat", std::ios::in);
    Record record{};

    int count = 0;
    while (sorted_file.read((char *) &record, SIZE(Record))) {
        std::cout << record.to_string() << std::endl;
        ++count;
    }

    std::cout << "Total of records read: " << count << std::endl;
    sorted_file.close();

    std::cout << "N: " << N<Record> << ", M: " << M<char[5]> << std::endl;
    std::cout << "DataPageSize: " << SIZE(DataPage<Record>) << " IndexPageSize: " << SIZE(IndexPage<char[5]>);
    return EXIT_SUCCESS;
}
