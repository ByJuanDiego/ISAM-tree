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
            heap_file_name, "./database/isam_indexed_by_dataId.dat", index);

//    std::cout << M<int> << " " << N<Pair<int>> << std::endl;
//    std::cout << sizeof(IndexPage<int>) << std::endl;
//    std::cout << sizeof(DataPage<Pair<int>>) << std::endl;

    func::clock clock;
    clock([&]() -> void {
              if (!isam) {
                  isam.create_index();
              } else {
                  std::cout << "isam tree already created" << std::endl;
              }
          }, "Build ISAM Tree"
    );

//    size_t sz = 0;
//    clock([&]() {
//        for (int i = 0; i < 105'000; ++i) {
//            sz += isam.search(i).size();
//        }
//    }, "Search all");
//    std::cout << "size: " << sz << std::endl;

//    clock([&](){
//        int dataId = 0;
//        std::cout << "data id: ";
//        std::cin >> dataId;
//
//        for (MovieRecord& movie : isam.search(dataId)) {
//            std::cout << movie.to_string() << std::endl;
//        }
//    }, "Search individual");

    int lower_bound = 0;
    int upper_bound = 0;
    std::cout << "lower bound: ";
    std::cin >> lower_bound;
    std::cout << "upper bound: ";
    std::cin >> upper_bound;

    clock([&]() {
        auto vec = isam.range_search(lower_bound, upper_bound);
        for (MovieRecord& movie: vec) {
            std::cout << movie.to_string() << std::endl;
        }
        std::cout << "total of records: " << vec.size() << std::endl;
    }, "Range search");

    return EXIT_SUCCESS;
}
