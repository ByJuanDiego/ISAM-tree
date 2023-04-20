#include <iostream>
#include "ISAM.hpp"
#include "record.hpp"

int main() {
    std::cout << DataPage<Record>::N << std::endl;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
