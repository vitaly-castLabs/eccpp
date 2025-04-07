// https://en.wikipedia.org/wiki/Kronecker_product

#ifndef ECCPP_KRON_H
#define ECCPP_KRON_H

#include "matrix.h"

namespace eccpp {

template <typename T, size_t Rows1, size_t Cols1, size_t Rows2, size_t Cols2>
matrix<T, Rows1 * Rows2, Cols1 * Cols2> kron(
    const matrix<T, Rows1, Cols1>& m1,
    const matrix<T, Rows2, Cols2>& m2) 
{
    matrix<T, Rows1 * Rows2, Cols1 * Cols2> result;

    for (size_t r1 = 0; r1 < Rows1; ++r1) {
        for (size_t c1 = 0; c1 < Cols1; ++c1) {
            const T val = m1[r1][c1];

            for (size_t r2 = 0; r2 < Rows2; ++r2)
                for (size_t c2 = 0; c2 < Cols2; ++c2)
                    result[r1 * Rows2 + r2][c1 * Cols2 + c2] = val * m2[r2][c2];
        }
    }

    return result;
}

}

#endif // #ifndef ECCPP_KRON_H
