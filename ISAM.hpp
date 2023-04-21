//
// Created by juandiego on 4/18/23.
//

#ifndef ISAM_HPP
#define ISAM_HPP

#include <functional>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


#define BUFFER_SIZE 256
#define SIZE(type) sizeof(type)
#define TYPE(type) decltype(type)
#define null (-1)

#define OPEN_FILES \
index_file1.open(index_file_name(1), flags); \
index_file2.open(index_file_name(2), flags); \
index_file3.open(index_file_name(3), flags); \
data_file.open(data_file_name, flags)

#define CLOSE_FILES \
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
    int n_keys{0};                      //< number of keys in the index page
    KeyType keys[M<KeyType>];           //< keys array
    long children[M<KeyType> + 1]{};    //< children physical position array

    void read(std::fstream &file) {
        file.read((char *) this, SIZE(IndexPage<KeyType>));
    }

    long locate(KeyType key) {
        int i = 0;
        for (; ((i < n_keys) && (key >= keys[i])); ++i);
        return children[i];
    }
};


//`N` stores the maximum number of records per data page
// It is calculated with the same logic as with `M`.
template<typename RecordType>
inline static constexpr int N = (BUFFER_SIZE - SIZE(long) - SIZE(int)) / SIZE(RecordType);

template<typename RecordType>
struct DataPage {
    RecordType records[N<RecordType>];
    long next{-1};
    int n_records{0};

    explicit DataPage() = default;

    std::fstream &read(std::fstream &file) {
        file.read((char *) this, SIZE(DataPage<RecordType>));
        return file;
    }

    void write(std::fstream &file) {
        file.write((char *) this, SIZE(DataPage<RecordType>));
    }
};

template<bool PrimaryKey, typename KeyType, typename RecordType, typename Index>
class ISAM {
private:

    /* Three levels of index files that stores index pages */
    std::fstream index_file1;
    std::fstream index_file2;
    std::fstream index_file3;

    std::function<std::string(int)> index_file_name = [](int i) {
        return "./database/index" + std::to_string(i) + ".dat";
    }; //< Gets the name of the ith index file


    /* Data pages is stored @ the last level, also the overflow pages are stored here */
    std::fstream data_file;

    std::string data_file_name; //< The name of the data file

    std::_Ios_Openmode flags = std::ios_base::in | std::ios_base::out | std::ios_base::binary; //< Open mode flags

    Index index; //< Receives a `RecordType` and returns its `KeyType` associated

    long locate(KeyType key) {
        /************************************** third (top) index level  **********************************************/
        IndexPage<KeyType> index1;
        index_file1.seekg(std::ios::beg);
        index1.read(index_file1);                     //< loads the unique index1 page in RAM
        long descend_to_index_2 = index1.locate(key); //< searches the physical pointer to descend to the second level

        /************************************** second index level ****************************************************/
        IndexPage<KeyType> index2;
        index_file2.seekg(descend_to_index_2);
        index2.read(index_file2);                     //< loads an index2 page in RAM
        long descend_to_index_3 = index2.locate(key); //< searches the pointer to descend to the last index level

        /************************************** first (base) index level **********************************************/
        IndexPage<KeyType> index3;
        index_file3.seekg(descend_to_index_3);
        index3.read(index_file3);                     //< loads an index3 page in RAM
        return index3.locate(key);                    //< searches the physical pointer to descend to a leaf page
    }

    void build_data_pages(std::ifstream &sorted_file, int data_pages) {
        data_file.open(data_file_name, std::ios::app);

        for (int i = 0; i < data_pages; ++i) {
            DataPage<RecordType> ram_page;
            RecordType record;

            while ((ram_page.n_records < N<RecordType>)) {
                if ((sorted_file >> record)) {
                    ram_page.records[ram_page.n_records++] = record;
                    continue;
                }
                if (ram_page.n_records == 0) {
                    data_file.close();
                    return;
                }
            }
            data_file.write((char *) &ram_page, SIZE(DataPage<RecordType>));
        }

        data_file.close();
    }

    void build_index_pages(int number_of_index_pages) {
        // TODO
    }

public:

    explicit ISAM(std::string file_name) : data_file_name(std::move(file_name)) {
        data_file.open(data_file_name, std::ios_base::app);
        data_file.close();
    }

    ISAM() = default;

    /******************************************************************************************************************/
    /* This member function must be called once, since it initializes the tree structure levels, which are static.    */
    /******************************************************************************************************************/
    void build_static_tree(const std::string &sorted_file_name) {
        std::ifstream sorted_file(sorted_file_name, std::ios::in | std::ios::binary);
        if (!sorted_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + std::string(sorted_file_name));
        }

        sorted_file.seekg(std::ios::end);
        long n_bytes = sorted_file.tellg();
        long n_records = n_bytes / SIZE(RecordType); //< Calculates the number of records in `sorted_file`
        long n_record_pages = std::ceil(double(n_records) / N<RecordType>); //< Calculates the number of data pages

        build_data_pages(sorted_file, n_record_pages); //< In charge of build the data pages @ `data_file`

        int r1 = std::ceil(double(n_record_pages) / (M<KeyType> + 1)); //< first (base) level number of index pages
        int r2 = std::ceil(double(r1) / (M<KeyType> + 1));             //< second (mid) level n...
        int r3 = std::ceil(double(r2) / (M<KeyType> + 1));             //< third  (top) level n...

        build_index_pages(r1);
        build_index_pages(r2);
        build_index_pages(r3);

        // If everything worked correctly, at this point a functional ISAM was initialized successfully
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
        OPEN_FILES; //< open the files

        // locates the physical position of the data page where the new record should be inserted
        long data_page_position = this->locate(index(record));
        DataPage<RecordType> data_page;

        long previous_data_page_position = null; //< indicates the previous data page position, initially there is no a previous page
        long non_full_data_page_position = null; //< helps to indicate if a not full page was found (to insert)
        bool repeated_key = false;               //< indicates if the record key to insert is already in the ISAM

        do {
            data_file.seekg(data_page_position); //< seeks the data page position
            data_page.read(data_file);           //< reads the page

            if ((data_page.n_records < N<RecordType>) && (!PrimaryKey)) {
                non_full_data_page_position = data_page_position;
                break; //< A place for the new non-pk record was found
            }
            for (int i = 0; i < data_page.n_records; ++i) {//< iterates over all the records @ the current page
                if (index(data_page.records[i]) != index(record)) {
                    continue;
                }
                repeated_key = true;
                if (PrimaryKey) {
                    break;
                }
            }

            previous_data_page_position = data_page_position; //< updates the `previous_page `
            data_page_position = data_page.next;              //< finally, updates the `data_page_position` to the next data page position.
        } while (data_page_position != null);

        if (PrimaryKey && repeated_key) {//< if a repeated key is found in a primary-key ISAM, an exception is thrown
            throw std::invalid_argument("Error: ISAM cannot insert a repeated primary key.");
        }

        if (non_full_data_page_position != null) {//< if a non-full data page was found, a `push_back` takes place
            data_page.records[data_page.n_records++] = record;
            data_file.seekp(non_full_data_page_position);
            data_page.write(data_file);
        } else {//< otherwise, a new `overflow` data page is created
            DataPage<RecordType> new_page;
            new_page.records[new_page.n_records++] = record;
            data_file.seekp(std::ios::end);

            long insertion_position = data_file.tellp();
            new_page.write(data_file);

            data_page.next = insertion_position;
            data_file.seekp(previous_data_page_position);
            data_page.write(data_file);
        }
        CLOSE_FILES; //< closes the files
    }

    std::vector<RecordType> search(KeyType key) {
        // ⬇️ inits an empty `std::vector` and open the files
        std::vector<RecordType> records;
        OPEN_FILES;

        // locates the physical position of the data page where the record to be searched is
        long data_page_position = this->locate(key);
        DataPage<RecordType> data_page;

        do {//< iterates over all the record pages until the end is reached
            data_file.seekg(data_page_position);
            data_page.read(data_file);

            for (int i = 0; i < data_page.count; ++i) { //< iterates the `leaf.count` records in the current page
                if (index(data_page.records[i]) != key) {
                    continue;
                }
                records.push_back(
                        data_page.records[i]); //< if the `record.key` equals `key`, pushes to the `std::vector`
            }

            data_page_position = data_page.next; //< updates the `data_page` pointer for the next iteration
        } while (data_page != null);

        CLOSE_FILES;
        return records;
        // ⬆️ closes each file and returns the result of the search.
    }

    std::vector<RecordType> range_search(KeyType lower_bound, KeyType upper_bound) {
        std::vector<RecordType> records;
        OPEN_FILES;

        long data_page = this->locate(lower_bound);
        // TODO

        CLOSE_FILES;
        return records;
    }
};

#endif //ISAM_HPP
