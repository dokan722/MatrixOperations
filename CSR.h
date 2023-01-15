#pragma once
#include <vector>
#include <stdexcept>
#include "Commons.h"

template <typename Field>
class CSR
{
private:
    size_t height;
    size_t width;

    std::vector<IndexedValue<Field>> matrix;
    std::vector<size_t> indices;
public:
    class NonZeroEntriesRows
    {
        typename std::vector<IndexedValue<Field>>::const_iterator iter;
        typename std::vector<IndexedValue<Field>>::const_iterator end_iter;
    public:
        class Iterator
        {
            typename std::vector<IndexedValue<Field>>::const_iterator iter;
        public:
            explicit Iterator(typename std::vector<IndexedValue<Field>>::const_iterator iter) : iter(iter) {}

            bool operator==(const Iterator &other) const = default;
            bool operator!=(const Iterator &other) const = default;
            void operator++(){ ++iter; }
            IndexedValue<Field> operator*()
            {
                return *iter;
            }
        };
        NonZeroEntriesRows(typename std::vector<IndexedValue<Field>>::const_iterator iter,
                       typename std::vector<IndexedValue<Field>>::const_iterator end_iter)
                       : iter(iter), end_iter(end_iter) {}

        Iterator begin()
        {
            return Iterator(iter);
        }

        Iterator end()
        {
            return Iterator(end_iter);
        }
    };

    class NonZeroEntriesColumns
    {
        size_t column_index;
        const CSR<Field> *matrix;
    public:
        class Iterator
        {
            size_t column_index;
            size_t row_index = 0;
            const CSR<Field> *matrix;

        public:
            Iterator(size_t const_index, size_t row_index, const CSR<Field> &matrix)
                    : column_index(const_index), row_index(row_index), matrix(&matrix) {}

            bool operator==(const Iterator &other) const = default;
            bool operator!=(const Iterator &other) const = default;
            void operator++()
            {
                row_index++;
            }
            IndexedValue<Field> operator*()
            {
                return {row_index, (*matrix)(row_index, column_index)};
            }
        };
        NonZeroEntriesColumns(const CSR<Field> &matrix, size_t const_index) : column_index(const_index), matrix(&matrix) {}

        Iterator begin()
        {
            return Iterator(column_index, 0, *matrix);
        }

        Iterator end()
        {
            return Iterator(column_index, (*matrix).get_height(), *matrix);
        }
    };

    CSR(size_t height, size_t width, std::vector<IndexedValue<Field>> matrixVec, std::vector<size_t> indicesVec)
    : height(height), width(width)
    {
        matrix.swap(matrixVec);
        indices.swap(indicesVec);
    }

    Field operator()(size_t i, size_t j) const
    {
        auto start = indices[i];
        auto end = indices[i+1];
        auto id = std::lower_bound(matrix.begin() + start, matrix.begin() + end, j,
                         [](const IndexedValue<Field> & elt, size_t needle) { return elt.index < needle; });
        return id->index == j ? id->value : Field{};
    }

    size_t get_height() const
    {
        return height;
    }

    size_t get_width() const
    {
        return width;
    }

    NonZeroEntriesRows iter_row_nonzero(size_t row_index) const
    {
        return NonZeroEntriesRows(matrix.begin() + indices[row_index], matrix.begin() + indices[row_index + 1]);
    }

    NonZeroEntriesColumns iter_column_nonzero(size_t column_index) const
    {
        return NonZeroEntriesColumns(*this, column_index);
    }
};
