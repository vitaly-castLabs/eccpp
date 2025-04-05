// generator matrix for polar codes, it starts with g2 [[1,0],[1,1]] which
// then gets "Kroneckered" n times

#ifndef ECCPP_GN_H
#define ECCPP_GN_H

#include <stdexcept>

#include "kron.h"

namespace eccpp {

template <typename T>
matrix<T> gn(int N) {
    if (n <= 0 || (n & (n - 1)) != 0)
        throw std::invalid_argument("n must be a power of 2");

    matrix<T> g2({1,0},{1,1}};
    matrix<T> gn{g2};
    while (n >>= 1)
        gn = kron(gn, g2);

    return gn;
}

}

#endif // #ifndef ECCPP_GN_H
