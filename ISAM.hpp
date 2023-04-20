//
// Created by juandiego on 4/18/23.
//

#ifndef ISAM_HPP
#define ISAM_HPP

#define BUFFER_SIZE 2048u
#define SIZE(type) sizeof(type)
#define TYPE(type) decltype(type)
#define null (-1)

#include <functional>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

/* `M` stores the number of keys in an index page
*
* BUFFER = (M * key) + (M + 1) * long + int
* BUFFER = (M * key) + (M * long) + long + int
* BUFFER = M * (key + long) + long_size + int
* BUFFER - long - int = M * (key + long)
* M = (BUFFER - long - int) / (key + long) */
template<typename KeyType>
inline static constexpr int M = (BUFFER_SIZE - SIZE(int) - SIZE(long)) / (SIZE(long) + SIZE(KeyType));

//`N` stores the maximum number of records per data page
// It is calculated with the same logic as with `M`.
template<typename RecordType>
inline static constexpr int N = (BUFFER_SIZE - SIZE(long) - SIZE(int)) / SIZE(RecordType);

template<typename KeyType>
struct IndexPage {
    int n_keys{0};                      //< number of keys in the index page
    KeyType keys[M<KeyType>];           //< keys array
    long children[M<KeyType> + 1]{};    //< children physical position array

    void read(std::fstream &file) {
        file.read((char *) this, BUFFER_SIZE);
    }

    long locate(KeyType key) {
        int i = 0;
        for (; ((i < n_keys) && (key >= keys[i])); ++i);
        return children[i];
    }
};

template<typename RecordType>
struct DataPage {
    RecordType *records = nullptr;
    long next{-1};
    int count{0};

    explicit DataPage() = default;

    void read(std::fstream &file) {
        file.read((char *) this, BUFFER_SIZE);
    }
};

template<typename KeyType, typename RecordType, typename Index>
class ISAM {
private:

    /* Three levels of index files that stores index pages */
    std::fstream index_file1;
    std::fstream index_file2;
    std::fstream index_file3;

    std::function<std::string(int)> index_file_name = [](int i) {
        return "./database/isam/index" + std::to_string(i) + ".dat";
    }; //< Gets the name of the ith index file


    /* Data pages is stored @ the last level, also the overflow pages are stored here */
    std::fstream data_file;

    std::string data_file_name; //< The name of the data file

    std::_Ios_Openmode flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary; //< Open mode flags

    Index index; //< Receives a `RecordType` and returns its `KeyType` associated

public:

    explicit ISAM(std::string file_name) : data_file_name(std::move(file_name)) {
        data_file.open(this->file_name, flags);
        data_file.close();
    }

    ISAM() = default;

    void insert(KeyType key) {

    }

    std::vector<RecordType> search(KeyType key) {
        // ⬇️ inits an empty `std::vector` and open the files
        std::vector<RecordType> records;
        index_file1.open(index_file_name(1), flags);
        index_file2.open(index_file_name(2), flags);
        index_file3.open(index_file_name(3), flags);
        data_file.open(data_file_name, flags);

        //----------------------------------- first index level --------------------------------------------------------
        IndexPage<KeyType> index1;
        index_file1.seekg(std::ios::beg);
        index1.read(index_file1);                     //< loads the unique index1 page in RAM
        long descend_to_index_2 = index1.locate(key); //< searches the physical pointer to descend to the second level

        //----------------------------------- second index level  ------------------------------------------------------
        IndexPage<KeyType> index2;
        index_file2.seekg(descend_to_index_2);
        index2.read(index_file2);                     //< loads an index2 page in RAM
        long descend_to_index_3 = index2.locate(key); //< searches the pointer to descend to the last index level

        //----------------------------------- third (last) index level -------------------------------------------------
        IndexPage<KeyType> index3;
        index_file3.seekg(descend_to_index_3);
        index3.read(index_file3);                     //< loads an index3 page in RAM
        long data_page = index3.locate(key);          //< searches the physical pointer to descend to a leaf page

        //----------------------------------- records ------------------------------------------------------------------
        DataPage<RecordType> leaf;

        do {                                          //< iterates over all the record pages until the end is reached
            data_file.seekg(data_page);
            leaf.read(data_file);

            for (int i = 0; i < leaf.count; ++i) {    //< iterates the `leaf.count` records in the current page
                if (index(leaf.records[i]) != key) {
                    continue;
                }
                records.push_back(leaf.records[i]);   //< if the `record.key` equals `key`, pushes to the `std::vector`
            }

            data_page = leaf.next;                    //< updates the `data_page` pointer for the next iteration
        } while (data_page != null);

        index_file1.close();
        index_file2.close();
        index_file3.close();
        data_file.close();
        return records;
        // ⬆️ closes each file and returns the result of the search.
    }
};

#endif //ISAM_HPP
