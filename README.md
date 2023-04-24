## Algorithms performance in function of disk accesses

$L := Number \ of \ index \ levels$

$K := Number \ of \ overflow \ pages \ chained \ with \ a \ data \ page$ 



|         Member Function          |    Performance     |                               Non Primary Key                               |                                                                           Primary Key                                                                            |
|:--------------------------------:|:------------------:|:---------------------------------------------------------------------------:|:----------------------------------------------------------------------------------------------------------------------------------------------------------------:|
|    ```locate(KeyType key)```     |  $\mathcal{O}(L)$  |                     Makes 1 access for each index level                     |                                                               Makes 1 access for each index level                                                                |
|    ```search(KeyType key)```     | $\mathcal{O}(L+K)$ |   Locates the ```key``` and then searches in ```all``` the chained pages    |            Locates the ```key``` and iterates over each chained data pages until the ```first``` record such that ```key = index(record)``` is found             |
| ```insert(RecordType& record)``` | $\mathcal{O}(L+K)$ | Locates the ```key``` and then looks for the ```first``` non-full data page | Locates the ```key``` and verifies in ```all``` chained pages if the ```key``` already exists and inserts the record at the ```first``` non-full data page found |

- When there is no space for a new record in a chain of record pages, a new overflow page is created and linked to previous overflow pages.
- ISAM tree is static; therefore, the number of disk accesses used to locate a key (known as $L$) is constant, as it never changes.

## References

- [1] Elmasri, R. & Navathe, S. (2010). Indexing Structures for Files. In [_Fundamentals of Database Systems_](https://docs.ccsu.edu/curriculumsheets/ChadTest.pdf) (pp. 643-646). Addison–Wesley.
