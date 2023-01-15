#pragma once

#include "DenseMatrix.h"
#include "CSR.h"
#include "SparseMatrix.h"

template<typename Matrix1, typename Matrix2>
struct OperationTraitsBinary
{};

template<typename Field, template<typename> typename OtherMatrix>
struct OperationTraitsBinary<OtherMatrix<Field>, DenseMatrix<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = DenseMatrix<Field>;
    using AdditionResultType = DenseMatrix<Field>;
};

template<typename Field, template<typename> typename OtherMatrix>
struct OperationTraitsBinary<DenseMatrix<Field>, OtherMatrix<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = DenseMatrix<Field>;
    using AdditionResultType = DenseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsBinary<DenseMatrix<Field>, DenseMatrix<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = DenseMatrix<Field>;
    using AdditionResultType = DenseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsBinary<CSR<Field>, CSR<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = GeneralSparseMatrix<Field>;
    using AdditionResultType = GeneralSparseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsBinary<GeneralSparseMatrix<Field>, GeneralSparseMatrix<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = GeneralSparseMatrix<Field>;
    using AdditionResultType = GeneralSparseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsBinary<CSR<Field>, GeneralSparseMatrix<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = GeneralSparseMatrix<Field>;
    using AdditionResultType = GeneralSparseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsBinary<GeneralSparseMatrix<Field>, CSR<Field>>
{
    using FieldType = Field;
    using MultiplicationResultType = GeneralSparseMatrix<Field>;
    using AdditionResultType = GeneralSparseMatrix<Field>;
};

template<typename Matrix>
struct OperationTraitsUnary
{};

template<typename Field>
struct OperationTraitsUnary<DenseMatrix<Field>>
{
    using FieldType = Field;
    using TransposeResultType = DenseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsUnary<CSR<Field>>
{
    using FieldType = Field;
    using TransposeResultType = GeneralSparseMatrix<Field>;
};

template<typename Field>
struct OperationTraitsUnary<GeneralSparseMatrix<Field>>
{
    using FieldType = Field;
    using TransposeResultType = GeneralSparseMatrix<Field>;
};


template <typename Matrix1, typename Matrix2>
typename OperationTraitsBinary<Matrix1, Matrix2>::MultiplicationResultType operator*(const Matrix1 &first_matrix, const Matrix2 &second_matrix)
{
    if (first_matrix.get_width() != second_matrix.get_height())
        throw std::invalid_argument("Matrices with such sizes cannot be multiplied");
    typename OperationTraitsBinary<Matrix1, Matrix2>::MultiplicationResultType result(first_matrix.get_height(), second_matrix.get_width());
    for (size_t i = 0; i < first_matrix.get_height(); ++i)
    {
        for (size_t j = 0; j < second_matrix.get_width(); ++j)
        {
            auto value = result(i, j);
            auto first_nonzero = first_matrix.iter_row_nonzero(i);
            auto first_iter = first_nonzero.begin();
            auto second_nonzero = second_matrix.iter_column_nonzero(j);
            auto second_iter = second_nonzero.begin();
            while (first_iter != first_nonzero.end() && second_iter != second_nonzero.end())
            {
                if((*first_iter).index == (*second_iter).index)
                {
                    value = value + (*first_iter).value * (*second_iter).value;
                    ++first_iter;
                    ++second_iter;
                }
                else
                {
                    if ((*first_iter).index > (*second_iter).index)
                        ++second_iter;
                    else
                        ++first_iter;
                }
            }
            result(i, j) = value;
        }
    }
    return result;
}

template <typename Matrix>
typename OperationTraitsUnary<Matrix>::TransposeResultType transpose(const Matrix &matrix)
{
    typename OperationTraitsUnary<Matrix>::TransposeResultType result(matrix.get_width(), matrix.get_height());
    for (int i = 0; i < matrix.get_height(); ++i)
    {
        auto nonzero = matrix.iter_row_nonzero(i);
        auto iter = nonzero.begin();
        while (iter != nonzero.end()) {
            result((*iter).index, i) = (*iter).value;
            ++iter;
        }
    }
    return result;
}

template <typename Matrix1, typename Matrix2>
typename OperationTraitsBinary<Matrix1, Matrix2>::AdditionResultType operator+(const Matrix1 &first_matrix, const Matrix2 &second_matrix)
{
    if (first_matrix.get_height() != second_matrix.get_height() || first_matrix.get_width() != second_matrix.get_width())
        throw std::invalid_argument("Matrices with such sizes cannot be added");
    typename OperationTraitsBinary<Matrix1, Matrix2>::AdditionResultType result(first_matrix.get_height(), second_matrix.get_width());
    for (int i = 0; i < first_matrix.get_height(); ++i)
    {
        auto first_nonzero = first_matrix.iter_row_nonzero(i);
        auto first_iter = first_nonzero.begin();
        auto second_nonzero = second_matrix.iter_row_nonzero(i);
        auto second_iter = second_nonzero.begin();
        while (first_iter != first_nonzero.end())
        {
            result(i, (*first_iter).index) = (*first_iter).value;
            ++first_iter;
        }
        while (second_iter != second_nonzero.end())
        {
            result(i, (*second_iter).index) = result(i, (*second_iter).index) + (*second_iter).value;
            ++second_iter;
        }
    }
    return result;
}




template<typename MatrixType>
void PrettyPrintMatrix(std::ostream & out, MatrixType const & m)
{
    for (size_t i = 0; i < m.get_height(); ++i)
    {
        for (size_t j = 0; j < m.get_height(); ++j)
        {
            out << m(i, j) << "\t";
        }
        out << std::endl;
    }
    out << std::endl;
}
