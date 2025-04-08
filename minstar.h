// "min-star" operation, used in LDPC decoding

#ifndef ECCPP_MINSTAR_H
#define ECCPP_MINSTAR_H

#include <cmath>
#include <algorithm>

namespace eccpp {

// this might be needed to match MATLAB behavior precisely:
// it seems to convert negative zero to just 0.0, however I have
// no idea if it matters at all
/*
static double matlab_fix_negative_zero(double val) {
    if (std::fpclassify(val) == FP_ZERO)
        return 0;

    return val;
}
*/

double minstar(double a, double b, bool approx) {
    if (approx || std::isinf(a) || std::isinf(b))
        return std::copysign(1, a) * std::copysign(1, b) * std::min(std::abs(a), std::abs(b));

    return 2 * std::atanh(std::tanh(a / 2) * std::tanh(b / 2));
}

} // namespace eccpp

#endif // ECCPP_MINSTAR_H
