#pragma once
#include <vector>
#include <stdexcept>
#include "Commons.h"

template <typename Field>
class DenseMatrix
{
private:
    size_t height;
    size_t width;

    std::vector<Field> matrix;
public:
    template <bool Row>
    class NonZeroEntries
    {
        size_t const_index;
        const DenseMatrix<Field> *matrix;
    public:
        class Iterator
        {
            size_t const_index;
            size_t var_index{};
            const DenseMatrix<Field> *matrix;
        public:
            Iterator(size_t const_index, size_t var_index, const DenseMatrix<Field> &matrix)
            : const_index(const_index), var_index(var_index), matrix(&matrix) {}

            bool operator==(const Iterator &other) const = default;
            bool operator!=(const Iterator &other) const = default;
            void operator++(){ ++var_index; }
            IndexedValue<Field> operator*()
            {
                if constexpr (Row)
                    return {var_index, (*matrix)(const_index, var_index)};
                return {var_index, (*matrix)(var_index, const_index)};
            }
        };
        NonZeroEntries(const DenseMatrix<Field> &matrix, size_t const_index) : const_index(const_index), matrix(&matrix) {}

        Iterator begin()
        {
            return Iterator(const_index, 0, *matrix);
        }

        Iterator end()
        {
            if constexpr (Row)
                return Iterator(const_index, (*matrix).get_width(), *matrix);
            return Iterator(const_index, (*matrix).get_height(), *matrix);
        }
    };

    DenseMatrix(size_t height, size_t width, Field def = {}) : height(height), width(width)
    {
        matrix.reserve(height * width);
        for (int i = 0; i < width * height; ++i)
            matrix.push_back(def);
    }

    DenseMatrix(size_t height, size_t width, std::vector<Field> vec) : height(height), width(width)
    {
        if (height * width != vec.size())
            throw std::invalid_argument("Vector size does not match matrix sizes.");
        matrix.swap(vec);
    }

    Field& operator()(size_t i, size_t j)
    {
        return matrix.at(i * width + j);
    }

    Field operator()(size_t i, size_t j) const
    {
        return matrix.at(i * width + j);
    }

    size_t get_height() const
    {
        return height;
    }

    size_t get_width() const
    {
        return width;
    }

    NonZeroEntries<true> iter_row_nonzero(size_t row_index) const
    {
        return NonZeroEntries<true>(*this, row_index);
    }

    NonZeroEntries<false> iter_column_nonzero(size_t column_index) const
    {
        return NonZeroEntries<false>(*this, column_index);
    }
};
