// https://en.wikipedia.org/wiki/Kronecker_product

#ifndef ECCPP_KRON_H
#define ECCPP_KRON_H

#include "matrix.h"

namespace eccpp {

template <typename T>
matrix<T> kron(const matrix<T>& A, const matrix<T>& B) {
    size_t a_row = A.rows();
    size_t a_col = A.cols();
    size_t b_row = B.rows();
    size_t b_col = B.cols();
    
    matrix<T> result(a_row * b_row, a_col * b_col);
    
    for (size_t i = 0; i < a_row; ++i) {
        for (size_t j = 0; j < a_col; ++j) {
            T a_val = A[i][j];
            for (size_t bi = 0; bi < b_row; ++bi) {
                for (size_t bj = 0; bj < b_col; ++bj)
                    result[i * b_row + bi][j * b_col + bj] = a_val * B[bi][bj];
            }
        }
    }
    
    return result;
}

}

#endif // #ifndef ECCPP_KRON_H
