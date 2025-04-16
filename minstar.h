// "min-star" operation, used in LDPC decoding

#ifndef ECCPP_MINSTAR_H
#define ECCPP_MINSTAR_H

#include <cmath>
#include <algorithm>

namespace eccpp {

// there's a bunch of subtle differences between C++'s std::copysign() and MATLAB's
// sign() around handling zeros and NaNs
static double matlab_sign(double a) {
    const auto a_type = std::fpclassify(a);
    // MATLAB's sign(0) and sign(-0) both return 0
    if (a_type == FP_ZERO)
        return 0;

    if (a_type == FP_NAN)
        return std::numeric_limits<double>::quiet_NaN();

    return a > 0 ? 1 : -1;
}

double minstar(double a, double b, bool approx) {
    if (approx || std::isinf(a) || std::isinf(b))
        return matlab_sign(a) * matlab_sign(b) * std::min(std::abs(a), std::abs(b));

    return 2 * std::atanh(std::tanh(a / 2) * std::tanh(b / 2));
}

} // namespace eccpp

#endif // ECCPP_MINSTAR_H
