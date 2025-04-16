// Implements equations (11b) and (12) from Balatsoukas-Stimming...
// https://arxiv.org/pdf/1401.3753

#ifndef ECCPP_PHI_H
#define ECCPP_PHI_H

#include <cmath>
#include <algorithm>

#include "./mdarray.h"

namespace eccpp {

template <typename T>
mdarray<T> phi(const mdarray<T>& PM_iminus1, const mdarray<T>& L_i, const mdarray<size_t>& u_i, bool approx_minstar) {
    mdarray<T> PM_i(PM_iminus1.dimensions());
    size_t size = PM_iminus1.dimensions()[0];
    if (approx_minstar) {
        for (size_t i = 0; i < size; ++i) {
            PM_i({i}) = PM_iminus1({i});

            bool flag = (0.5 * (1 - (L_i({i}) > 0 ? true : (L_i({i}) < 0 ? -1 : 0)))) != u_i({i});
            if (flag)
                PM_i({i}) = PM_i({i}) + std::abs(L_i({i}));
        }
    }
    else {
        for (size_t i = 0; i < size; ++i) {
            T exponent = -(1 - 2 * static_cast<T>(u_i({i}))) * L_i({i});
            PM_i({i}) = PM_iminus1({i}) + std::log1p(std::exp(exponent));
        }
    }

    return PM_i;
}

} // namespace eccpp

#endif // ECCPP_PHI_H
