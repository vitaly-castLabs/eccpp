#ifndef ECCPP_HAMWEIGHT_H
#define ECCPP_HAMWEIGHT_H

#include <vector>
#include <cassert>

#include "mdarray.h"

namespace eccpp {

template <typename T>
size_t hamweight(const std::vector<T>& a) {
    size_t weight = 0;
    for (const auto a_val: a) {
        assert(a_val == 0 || a_val == 1);

        if (a_val)
            ++weight;
    }
    return weight;
}

template <typename T>
size_t hamweight(const mdarray<T>& a) {
    size_t weight = 0;
    const auto& dims = a.dimensions();
    std::vector<size_t> indices(dims.size(), 0);

    while (true) {
        const T a_val = a(indices);
        assert(a_val == 0 || a_val == 1);

        // Increment distance if elements differ
        if (a_val)
            ++weight;

        // Increment indices in row-major order
        bool done = true;
        for (int i = dims.size() - 1; i >= 0; --i) {
            if (indices[i] + 1 < dims[i]) {
                ++indices[i];
                done = false;
                break;
            }
            else
                indices[i] = 0;
        }
        if (done)
            break;
    }

    return weight;
}

} // namespace eccpp

#endif // ECCPP_HAMWEIGHT_H
