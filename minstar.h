// "min-star" operation, used in LDPC decoding

#ifndef ECCPP_MINSTAR_H
#define ECCPP_MINSTAR_H

#include <cmath>

#include "./sign.h"

namespace eccpp {

double minstar(double a, double b, bool approx) {
    if (approx || std::isinf(a) || std::isinf(b))
        return sign(a) * sign(b) * std::min(std::abs(a), std::abs(b));

    return 2 * std::atanh(std::tanh(a / 2) * std::tanh(b / 2));
}

} // namespace eccpp

#endif // ECCPP_MINSTAR_H
