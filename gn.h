// generator matrix for polar codes, it starts with g2 [[1,0],[1,1]] which
// then gets "Kroneckered" until it's n x n in size

#ifndef ECCPP_GN_H
#define ECCPP_GN_H

#include <vector>

#include "kron.h"

namespace eccpp {

static mdarray<int> gn(size_t n) {
    if (!n || (n & (n - 1)) != 0)
        throw std::invalid_argument("n must be a power of 2");

    if (n == 1) {
        mdarray<int> g1({1});
        g1({0}) = 1;
        return g1;
    }

    mdarray<int> g2({2, 2});
    g2({0, 0}) = 1;
    g2({0, 1}) = 0;
    g2({1, 0}) = 1;
    g2({1, 1}) = 1;

    auto gn = g2;
    while ((n >>= 1) > 1)
        gn = kron(gn, g2);

    return gn;
}

} // namespace eccpp

#endif // ECCPP_GN_H
