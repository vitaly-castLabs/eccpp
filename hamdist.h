#ifndef ECCPP_HAMDIST_H
#define ECCPP_HAMDIST_H

#include <vector>
#include <cassert>

#include "mdarray.h"

namespace eccpp {

template <typename T>
size_t hamdist(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size())
        throw std::invalid_argument("Hamming distance: dimensions must match");

    size_t distance = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        const T a_val = a[i];
        const T b_val = b[i];
        assert(a_val == 0 || a_val == 1);
        assert(b_val == 0 || b_val == 1);

        if (a_val != b_val)
            ++distance;
    }
    return distance;
}

template <typename T>
size_t hamdist(const mdarray<T>& a, const mdarray<T>& b) {
    if (a.dimensions() != b.dimensions())
        throw std::invalid_argument("Hamming distance: dimensions must match");

    size_t distance = 0;
    const auto& dims = a.dimensions();
    std::vector<size_t> indices(dims.size(), 0);

    while (true) {
        const T a_val = a(indices);
        const T b_val = b(indices);
        assert(a_val == 0 || a_val == 1);
        assert(b_val == 0 || b_val == 1);

        // Increment distance if elements differ
        if (a_val != b_val)
            ++distance;

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

    return distance;
}

} // namespace eccpp

#endif // ECCPP_HAMDIST_H
