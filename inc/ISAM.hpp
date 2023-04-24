//
// Created by juandiego on 4/18/23.
//

#ifndef ISAM_HPP
#define ISAM_HPP

#include <cmath>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.hpp"

#define BUFFER_SIZE 128
#define SIZE(T) sizeof(T)
#define DISK_NULL (-1)

#define INT_POW(BASE, EXP) (static_cast<int>(std::pow(BASE, EXP)))

#define OPEN_FILES(MODE)                        \
    index_file1.open(index_file_name(1), MODE); \
    index_file2.open(index_file_name(2), MODE); \
    index_file3.open(index_file_name(3), MODE); \
    data_file.open(data_file_name, MODE)

#define CLOSE_FILES      \
    index_file1.close(); \
    index_file2.close(); \
    index_file3.close(); \
    data_file.close()


/* `M` stores the number of keys in an index page
*
* BUFFER = (M * key) + (M + 1) * long + int
* BUFFER = (M * key) + (M * long) + long + int
* BUFFER = M * (key + long) + long_size + int
* BUFFER - long - int = M * (key + long)
* M = (BUFFER - long - int) / (key + long) */
template<typename KeyType>
inline static constexpr int M = (BUFFER_SIZE - SIZE(int) - SIZE(long)) / (SIZE(long) + SIZE(KeyType));

template<typename KeyType>
struct IndexPage {
    int n_keys{0};                  //< number of keys in the index page
    KeyType keys[M<KeyType>]{};     //< keys array
    long children[M<KeyType> + 1]{};//< children physical position array

    long locate(KeyType key, std::function<bool(KeyType, KeyType)> greater) {
        int i = 0;
        for (; ((i < n_keys) && !greater(keys[i], key)); ++i);
        return children[i];
    }
};


//`N` stores the maximum number of records per data page
// It is calculated with the same logic as with `M`.
template<typename RecordType>
inline static constexpr int N = (BUFFER_SIZE - SIZE(long) - SIZE(int)) / SIZE(RecordType);

template<typename RecordType>
struct DataPage {
    RecordType records[N<RecordType>]{};
    long next{-1};
    int n_records{0};

    explicit DataPage() = default;
};

template<bool PrimaryKey, typename KeyType, typename RecordType, typename Index, typename Greater>
class ISAM {
private:
    /* Three levels of index files that stores index pages */
    std::fstream index_file1;
    std::fstream index_file2;
    std::fstream index_file3;
    std::function<std::string(int)> index_file_name = [](int i, const std::string &relative_path = "../database") {
        return relative_path + "/index" + std::to_string(i) + ".dat";
    };//< Gets the name of the ith index file

    /* Data pages are stored @ the last level */
    std::fstream data_file;
    std::string data_file_name; //< The name of the data file

    std::_Ios_Openmode flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;  //< Open mode flags

    Index index;        //< Receives a `RecordType` and returns its `KeyType` associated
    Greater greater;    //< Receives two `RecordTypes` and returns `true` if a.key > b.key and `false` otherwise

    long locate(KeyType key) {
        /************************************** third (top) index level  **********************************************/
        IndexPage<KeyType> index1;
        index_file1.seekg(std::ios::beg);
        index_file1.read((char *) &index1, SIZE(IndexPage<KeyType>));//< loads the unique index1 page in RAM
        long descend_to_index_2 = index1.locate(key, greater);
        //^ searches the physical pointer to descend to the second level

        /************************************** second index level ****************************************************/
        IndexPage<KeyType> index2;
        index_file2.seekg(descend_to_index_2);
        index_file2.read((char *) &index2, SIZE(IndexPage<KeyType>));//< loads an index2 page in RAM
        long descend_to_index_3 = index2.locate(key, greater);
        //^ searches the pointer to descend to the last index level

        /************************************** first (base) index level **********************************************/
        IndexPage<KeyType> index3;
        index_file3.seekg(descend_to_index_3);
        index_file3.read((char *) &index3, SIZE(IndexPage<KeyType>));//< loads an index3 page in RAM
        long page_position = index3.locate(key, greater);
        //^ searches the physical pointer to descend to a leaf page
        return page_position;
    }

    void init_data_pages(std::ifstream &sorted_file, int data_pages,
                         KeyType *l1_keys, KeyType *l2_keys, KeyType *l3_keys) {
        int l3 = 0, l2 = 0;
        int k = 0, m = 0, n = 0;

        for (int i = 0; i < data_pages; ++i) {
            DataPage<RecordType> ram_page;

            for (int j = 0; j < N<RecordType>; ++j) {
                RecordType record;
                sorted_file.read((char *) &record, sizeof(RecordType));
                ram_page.records[j] = record;
            }

            ram_page.n_records = N<RecordType>;
            data_file.write((char *) &ram_page, SIZE(DataPage<RecordType>));

            /* This part of the code stores the key of the first record of each data page in vectors in order to      */
            /* assign the keys in each index level later in the process of ISAM-tree initialization.                  */
            auto key = index(ram_page.records[0]);

            if (i == 0) {//< Skips the first data page (not belongs to any key level)
                continue;
            }

            if (l3 < M<KeyType>) {
                func::copy(l3_keys[k++], key);
                ++l3;
            } else {
                l3 = 0;
                if (l2 < M<KeyType>) {
                    func::copy(l2_keys[m++], key);
                    ++l2;
                } else {
                    l2 = 0;
                    func::copy(l1_keys[n++], key);
                }
            }
        }
    }

    void init_index_pages(std::fstream &index_file, int level, int M, KeyType *keys) {
        int number_of_pages = INT_POW((M + 1), level);
        long pointed_data_size = (level != 2) ? SIZE(IndexPage<KeyType>) : SIZE(DataPage<RecordType>);
        long children = 0;
        int k = 0;

        for (int i = 0; i < number_of_pages; ++i) {
            IndexPage<KeyType> ram_page;

            int j = 0;
            for (; j < M; ++j) {
                func::copy(ram_page.keys[j], keys[k++]);
                ram_page.children[j] = ((children++) * pointed_data_size);
            }

            ram_page.n_keys = M;
            ram_page.children[j] = (children++) * pointed_data_size;
            index_file.write((char *) &ram_page, SIZE(IndexPage<KeyType>));
        }
    }


    void locate_insertion_pk(long seek, RecordType &record, long &non_full, long &prev) {
        DataPage<RecordType> page;

        do {
            data_file.seekg(seek);
            data_file.read((char *) &page, SIZE(DataPage<RecordType>));

            for (int i = 0; i < page.n_records; ++i) {
                if (!greater(index(page.records[i]), index(record)) &&
                    !greater(index(record), index(page.records[i]))) {
                    CLOSE_FILES;
                    throw std::invalid_argument("Error: ISAM cannot insert a repeated primary key.");
                }
            }

            non_full = (page.n_records < N<RecordType> && non_full == DISK_NULL) ? seek : non_full;
            prev = seek;
            seek = page.next;
        } while (seek != DISK_NULL);
    }

    void locate_insertion_non_pk(long seek, long &non_full, long &prev) {
        DataPage<RecordType> page;

        do {
            data_file.seekg(seek);
            data_file.read((char *) &page, SIZE(DataPage<RecordType>));

            if (page.n_records < N<RecordType>) {
                non_full = seek;
                return;
            }

            prev = seek;
            seek = page.next;
        } while (seek != DISK_NULL);
    }

public:
    explicit ISAM(std::string file_name, Index index, Greater greater)
            : data_file_name(std::move(file_name)), index(index),
              greater(greater) {
        data_file.open(data_file_name, std::ios_base::app);
        data_file.close();
    }

    ISAM() = default;

    /******************************************************************************************************************/
    /* This member function meets the following requirements                                                          */
    /*   • It must be called once, since it initializes the tree structure levels, which are static.                  */
    /*   • It assumes that the `sorted_file` has exactly N * (M+1)^3 records, in order to generate a full ISAM-tree   */
    /******************************************************************************************************************/
    void build_static_tree(const std::string &sorted_file_name) {
        std::ifstream sorted_file(sorted_file_name, std::ios::binary);
        if (!sorted_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + std::string(sorted_file_name));
        }
        OPEN_FILES(std::ios::app);

        int max_n_children = M<KeyType> + 1;        //< Maximum number of children per index page
        int n_pages = INT_POW(max_n_children, 3);   //< Total number of record pages in full ISAM-tree

        // Reserves memory for the keys
        auto *first_level_keys = new KeyType[INT_POW(max_n_children, 0) * M<KeyType>];
        auto *second_level_keys = new KeyType[INT_POW(max_n_children, 1) * M<KeyType>];
        auto *third_level_keys = new KeyType[INT_POW(max_n_children, 2) * M<KeyType>];

        // First, creates all the data pages with all the records and appends them to `data_file`.
        // Also, stores the information of the keys in each level in arrays.
        init_data_pages(sorted_file, n_pages, first_level_keys, second_level_keys, third_level_keys);
        /* index1    (still empty)
         * index2    (still empty)
         * index3    (still empty)
         * data_file [r_{1}, r_{2}, ..., r_{N-1}, r_{N}][*] ... [*][*]
         */

        // Then, creates the index pages for each level and fills them with his correspondents keys
        init_index_pages(index_file3, 2, M<KeyType>, third_level_keys);
        init_index_pages(index_file2, 1, M<KeyType>, second_level_keys);
        init_index_pages(index_file1, 0, M<KeyType>, first_level_keys);
        /* index1:                      [k_1          k_2           ...         k_{M-1}             k_M]
         *                             /               |            ...            |                    \
         * index2:           [x < k_1]          [k_1 <= x < k_2]    ...     [k_{M-2} <= x < k_{M-1}]    [x >= k_M]
         *                 /     ...    \        /    ...    \      ...    /    ...    \               /    ...   \
         * index3:      [.]      ...     [.]   [.]    ...     [.]   ...  [.]    ...     [.]         [.]     ...    [.]
         *             /  \      ...    /  \  /  \    ...    /  \       /  \    ...    /  \        /  \     ...   /  \
         * data:      [*][*]     ...   [*][*][*][*]   ...   [*][*]     [*][*]   ...   [*][*]      [*][*]    ...  [*][*]
         */

        delete[] first_level_keys;
        delete[] second_level_keys;
        delete[] third_level_keys;

        sorted_file.close();
        CLOSE_FILES;
    }

    std::vector<RecordType> search(KeyType key) {
        // ⬇️ inits an empty `std::vector` and open the files
        std::vector<RecordType> records;
        OPEN_FILES(flags);

        // locates the physical position of the data page where the record to be searched is
        long seek = this->locate(key);
        DataPage<RecordType> page;

        do {
            data_file.seekg(seek);
            data_file.read((char *) &page, SIZE(DataPage<RecordType>));
            for (int i = 0; i < page.n_records; ++i) {  //< iterates the leaf records in the current page
                if (!greater(index(page.records[i]), key) && !greater(key, index(page.records[i]))) {
                    records.push_back(page.records[i]); //< if the `record.key` equals `key`, pushes to the `vector`

                    if (PrimaryKey) {// if indexing a primary-key, it breaks the loop (the unique record was found).
                        break;
                    }
                }
            }
            seek = page.next; //< `page.next` probably points to an overflow page (or to nothing)
        } while (seek != DISK_NULL);

        CLOSE_FILES;
        return records;
        // ⬆️ closes each file and returns the result of the search.
    }

    std::vector<RecordType> range_search(KeyType lower_bound, KeyType upper_bound) {
        std::vector<RecordType> records;
        OPEN_FILES(flags);

        // locates the physical position of the data page where the `lower_bound` is located
        long seek_page = this->locate(lower_bound);
        long n_static_pages = INT_POW(M<KeyType> + 1, 3);
        DataPage<RecordType> page;
        bool any_found;

        do {
            any_found = false;
            data_file.seekg(seek_page);
            data_file.read((char *) &page, SIZE(DataPage<RecordType>));

            for (int i = 0; i < page.n_records; ++i) {
                if (greater(index(page.records[i]), lower_bound) &&
                    greater(upper_bound, index(page.records[i]))) {
                    records.push_back(page.records[i]);
                    any_found = true;
                }
            }

            DataPage<RecordType> overflow;
            long seek_overflow = page.next;
            while (seek_overflow != DISK_NULL) {
                data_file.seekg(seek_overflow);
                data_file.read((char *) &overflow, SIZE(DataPage<RecordType>));

                for (int j = 0; j < overflow.n_records; ++j) {
                    if (greater(index(overflow.records[j]), lower_bound) &&
                        greater(upper_bound, index(overflow.records[j]))) {
                        records.push_back(overflow.records[j]);
                        any_found = true;
                    }
                }
                seek_overflow = overflow.next;
            }

            seek_page += SIZE(DataPage<RecordType>);
        } while (any_found && (seek_page != n_static_pages * SIZE(DataPage<RecordType>)));

        CLOSE_FILES;
        return records;
    }

    /******************************************************************************************************************/
    /* This member function makes the following assumption                                                            */
    /*   • The three indexing levels are already created, so the criteria for descending in the tree is well-defined. */
    /*                                                                                                                */
    /* In addition, by ISAM definition we have                                                                        */
    /*   • The three indexing levels are totally static, so the tree do not admit the split of indexing pages.        */
    /*   • If the selected insertion page is full, a data page split occurs.                                          */
    /******************************************************************************************************************/
    void insert(RecordType &record) {
        OPEN_FILES(flags);//< open the files

        // locates the physical position of the data page where the new record should be inserted
        long seek = this->locate(index(record));
        long non_full = DISK_NULL;
        long prev = DISK_NULL;

        PrimaryKey ? locate_insertion_pk(seek, record, non_full, prev) : locate_insertion_non_pk(seek, non_full, prev);
        //^ At the end of this call, `non_full` contains the physical position of the first DataPage
        //  found whose number of records does not exceed the maximum allowed (`N`)
        //  and `prev` contains the physical position of the last DataPage before DISK_NULL was found (if is required).

        if (non_full != DISK_NULL) {
            DataPage<RecordType> non_full_page;
            data_file.seekg(non_full);
            data_file.read((char *) &non_full_page, SIZE(DataPage<RecordType>));
            non_full_page.records[non_full_page.n_records++] = record;
            data_file.seekp(non_full);
            data_file.write((char *) &non_full_page, SIZE(DataPage<RecordType>));
        } else {
            data_file.close();
            data_file.open(data_file_name, std::ios::app);
            DataPage<RecordType> new_page;
            new_page.records[new_page.n_records++] = record;
            long pos = data_file.tellp();
            data_file.write((char *) &new_page, SIZE(DataPage<RecordType>));
            data_file.close();

            data_file.open(data_file_name, flags);
            DataPage<RecordType> prev_page;
            data_file.seekg(prev);
            data_file.read((char *) &prev_page, SIZE(DataPage<RecordType>));
            prev_page.next = pos;
            data_file.seekp(prev);
            data_file.write((char *) &prev_page, SIZE(DataPage<RecordType>));
        }

        CLOSE_FILES;//< closes the files
    }
};

#endif//ISAM_HPP
