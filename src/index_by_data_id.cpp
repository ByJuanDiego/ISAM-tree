//
// Created by juandiego on 5/2/23.
//

//
// Created by juandiego on 5/1/23.
//

#include "../inc/ISAM.hpp"
#include "../inc/record.hpp"

int main() {
    std::string heap_file_name = "./database/movies_and_series.dat";

    std::function<int(MovieRecord &)> index = [](MovieRecord &record) -> int {
        return record.dataId;
    };

    ISAM<true, int, MovieRecord, std::function<int(MovieRecord &)>> isam(
            "./database/isam_indexed_by_dataId.dat", index);

    func::clock clock;
    clock([&]() -> void {
              if (!isam) {
                  isam.build_static_tree(heap_file_name);
              } else {
                  std::cout << "isam tree already created" << std::endl;
              }
          }, "Build ISAM Tree"
    );

    int dataId;
    std::cout << "Enter the dataId: ";
    std::cin >> dataId;

    clock([&]() -> void {
        for (MovieRecord &movie: isam.search(dataId, heap_file_name)) {
            std::cout << movie.to_string() << std::endl;
        }
    }, "Search by Id ISAM");

    return EXIT_SUCCESS;
}
