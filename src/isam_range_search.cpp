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
                  std::cout << "the isam tree do not exists" << std::endl;
                  isam.create_index();
              } else {
                  std::cout << "isam tree already created" << std::endl;
              }
          }, "Build ISAM Tree"
    );

    int lower_bound = 0;
    int upper_bound = 0;
    std::cout << "lower bound: ";
    std::cin >> lower_bound;
    std::cout << "upper bound: ";
    std::cin >> upper_bound;

    clock([&]() {
        auto records = isam.range_search(lower_bound, upper_bound);
        for (MovieRecord &movie: records) {
            std::cout << movie.to_string() << std::endl;
        }
        std::cout << "total of records: " << records.size() << std::endl;
    }, "Range-search");

    return EXIT_SUCCESS;
}
