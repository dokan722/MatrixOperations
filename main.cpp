#include <iostream>
#include <vector>


#include "MatrixOperations.h"

int main() {
    std::vector<int> testVec1{1,2,3,4,5,6,7,8,9};
    std::vector<int> testVec2{1, 0, 0, 0, 1, 0, 0, 0, 1};
    DenseMatrix test1(3, 3, testVec1);
    DenseMatrix test2(3, 3, testVec2);
    for (int i = 0; i < 3; ++i)
    {
        for (auto val : test1.iter_row_nonzero(i))
        {
            std::cerr << val.value << "\t";
        }
        std::cerr << "\n";
    }
    for (int i = 0; i < 3; ++i)
    {
        for (auto val : test1.iter_column_nonzero(i))
        {
            std::cerr << val.value << "\t";
        }
        std::cerr << "\n";
    }
    DenseMatrix test3 = test1 * test2;
    std::cerr << std::endl;
    for (int i = 0; i < 3; ++i)
    {
        for (auto val : test3.iter_row_nonzero(i))
        {
            std::cerr << val.value << "\t";
        }
        std::cerr << "\n";
    }

    CSR<int> test4{10, 10,
                   {{0, 1}, {1, 1}, {2, 1}},
                   {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}};
    CSR<int> test5{10, 10,
                   {{0, 1}, {0, 1}, {0, 1}},
                   {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3}};
    auto test6 = test4 * test5;
    PrettyPrintMatrix(std::cerr, test6);

    GeneralSparseMatrix<int> test7(10, 10,
                                   {{{0, 0}, 1}, {{0, 1}, 1}, {{0, 2}, 1}});
    GeneralSparseMatrix<int> test8(10, 10,
                                   {{{0, 0}, 1}, {{1, 0}, 1}, {{2, 0}, 1}});
    auto test9 = test7 * test8;
    PrettyPrintMatrix(std::cerr, test9);

    auto test10 = transpose(test4);
    PrettyPrintMatrix(std::cerr, test10);

    auto test11 = test4 + test5;
    PrettyPrintMatrix(std::cerr, test11);

    return 0;
}
