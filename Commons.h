#pragma once

#include <cstddef>

template <typename Field>
struct IndexedValue
{
    size_t index;
    Field value;
};