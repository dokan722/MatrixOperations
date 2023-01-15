#pragma once
#include <vector>
#include <set>
#include <stdexcept>
#include <map>
#include "Commons.h"

template <typename Field>
class GeneralSparseMatrix
{
public:
    struct Entry
    {
        size_t row;
        size_t column;
        std::strong_ordering operator<=>(const Entry & other) const = default;
    };

    friend class ElementProxy;
    class ElementProxy
    {
    public:
        ElementProxy(GeneralSparseMatrix &matrix, Entry location):
            matrix(&matrix),
            location(location)
        {}

        operator Field() const
        {
            auto it = matrix->values.find(location);
            return it == matrix->values.end() ? Field{} : it->second;
        }

        Field operator=(Field value)
        {
            if (value == Field{})
                return value;
            auto it = matrix->values.find(location);
            if (it == matrix->values.end()) {
                matrix->values.insert({location, value});
                matrix->nonzero_by_row[location.row].insert(location.column);
                matrix->nonzero_by_column[location.column].insert(location.row);
            } else {
                it->second = value;
            }
            return value;
        }

    private:
        GeneralSparseMatrix *matrix;
        Entry location;
    };

    size_t height;
    size_t width;
private:
    std::vector<std::set<size_t>> nonzero_by_row;
    std::vector<std::set<size_t>> nonzero_by_column;
    std::map<Entry, Field> values;
public:
    template <bool Row>
    class NonZeroEntries
    {
        size_t const_index;
        std::set<size_t>::const_iterator iter;
        std::set<size_t>::const_iterator end_iter;
        const GeneralSparseMatrix<Field> *matrix;
    public:
        class Iterator
        {
            size_t const_index;
            std::set<size_t>::const_iterator var_index;
            const GeneralSparseMatrix<Field> *matrix;
        public:
            Iterator(size_t const_index, std::set<size_t>::const_iterator var_index, const GeneralSparseMatrix<Field> &matrix)
                    : const_index(const_index), var_index(var_index), matrix(&matrix) {}

            bool operator==(const Iterator &other) const = default;
            bool operator!=(const Iterator &other) const = default;
            void operator++(){ ++var_index; }
            IndexedValue<Field> operator*()
            {
                if constexpr (Row)
                    return {*var_index, (*matrix)(const_index, *var_index)};
                return {*var_index, (*matrix)(*var_index, const_index)};
            }
        };
        NonZeroEntries(const GeneralSparseMatrix<Field> &matrix, size_t const_index,
                       std::set<size_t>::const_iterator iter, std::set<size_t>::const_iterator end_iter)
        : const_index(const_index), matrix(&matrix), iter(iter), end_iter(end_iter) {}

        Iterator begin()
        {
            return Iterator(const_index, iter, *matrix);
        }

        Iterator end()
        {
            return Iterator(const_index, end_iter, *matrix);
        }
    };

    GeneralSparseMatrix(size_t height, size_t width) : height(height), width(width)
    {
        nonzero_by_row.resize(height);
        nonzero_by_column.resize(width);
    }

    GeneralSparseMatrix(size_t height, size_t width, std::map<Entry, Field> valuesMap) : height(height), width(width)
    {
        nonzero_by_row.resize(height);
        nonzero_by_column.resize(width);
        for (auto entry : valuesMap)
        {
            Entry pos = entry.first;
            nonzero_by_row[pos.row].insert(pos.column);
            nonzero_by_column[pos.column].insert(pos.row);
        }
        values.swap(valuesMap);
    }

    ElementProxy operator()(size_t i, size_t j)
    {
        return ElementProxy(*this, {i, j});
    }

    Field operator()(size_t i, size_t j) const
    {
        auto it = values.find({i, j});
        return it == values.end() ? Field{} : it->second;
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
        return NonZeroEntries<true>(*this, row_index, nonzero_by_row[row_index].begin(), nonzero_by_row[row_index].end());
    }

    NonZeroEntries<false> iter_column_nonzero(size_t column_index) const
    {
        return NonZeroEntries<false>(*this, column_index, nonzero_by_column[column_index].begin(), nonzero_by_column[column_index].end());
    }
};
