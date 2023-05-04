//
// Created by juandiego on 5/2/23.
//

#include "../inc/ISAM.hpp"
#include "../inc/record.hpp"

int main() {
    std::string heap_file_name = "./database/movies_and_series.dat";
    std::string attribute = "dataId";

    std::function<int(MovieRecord &)> index = [](MovieRecord &record) -> int { return record.dataId; };
    ISAM<true, int, MovieRecord> isam(heap_file_name, attribute, index);

    func::clock clock;
    clock([&]() -> void {
              if (!isam) {
                  std::cout << "the ISAM tree do not exists" << std::endl;
                  isam.create_index();
              } else {
                  std::cout << "ISAM tree already created" << std::endl;
              }
          }, "Build ISAM Tree"
    );

    int dataId = 0;
    std::cout << "Enter data id: ";
    std::cin >> dataId;

    clock([&]() {
        for (MovieRecord &movie: isam.search(dataId)) {
            std::cout << movie.to_string() << std::endl;
        }
    }, "Key-Based search");

    return EXIT_SUCCESS;
}
