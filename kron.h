// https://en.wikipedia.org/wiki/Kronecker_product

#ifndef ECCPP_KRON_H
#define ECCPP_KRON_H

#include "mdarray.h"

namespace eccpp {

template <typename T>
mdarray<T> kron(const mdarray<T>& m1, const mdarray<T>& m2) {
    if (m1.dimensions().size() != 2 || m2.dimensions().size() != 2)
        throw std::invalid_argument("Kronecker product requires 2D matrices");

    const size_t rows1 = m1.dimensions()[0];
    const size_t cols1 = m1.dimensions()[1];
    const size_t rows2 = m2.dimensions()[0];
    const size_t cols2 = m2.dimensions()[1];

    mdarray<T> result({rows1 * rows2, cols1 * cols2});

    for (size_t r1 = 0; r1 < rows1; ++r1) {
        for (size_t c1 = 0; c1 < cols1; ++c1) {
            const T val = m1({r1, c1});

            for (size_t r2 = 0; r2 < rows2; ++r2) {
                for (size_t c2 = 0; c2 < cols2; ++c2) {
                    result({r1 * rows2 + r2, c1 * cols2 + c2}) = val * m2({r2, c2});
                }
            }
        }
    }

    return result;
}

} // namespace eccpp

#endif // ECCPP_KRON_H
