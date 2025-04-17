// MATLAB sign function

#ifndef ECCPP_SIGN_H
#define ECCPP_SIGN_H

#include <cmath>

namespace eccpp {

// there's a bunch of subtle differences between C++'s std::copysign() and MATLAB's
// sign() around handling zeros and NaNs
template<typename T>
static T sign(T a) {
    const auto a_type = std::fpclassify(a);
    // MATLAB's sign(0) and sign(-0) both return 0
    if (a_type == FP_ZERO)
        return 0;

    if (a_type == FP_NAN)
        return std::numeric_limits<T>::quiet_NaN();

    return a > 0 ? 1 : -1;
}

} // namespace eccpp

#endif // ECCPP_SIGN_H
