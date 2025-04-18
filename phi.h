// Implements equations (11b) and (12) from Balatsoukas-Stimming...
// https://arxiv.org/pdf/1401.3753

#ifndef ECCPP_PHI_H
#define ECCPP_PHI_H

#include <cmath>
#include <algorithm>

#include "./mdarray.h"
#include "./sign.h"

namespace eccpp {

template <typename T>
mdarray<T> phi(const mdarray<T>& PM_iminus1, const mdarray<T>& L_i, const T u_i, bool approx_minstar) {
    // Check if all input dimensions match
    if (PM_iminus1.dimensions() != L_i.dimensions())
        throw std::invalid_argument("phi: Input dimensions must be identical.");

    // Initialize PM_i as a copy of PM_iminus1
    mdarray<T> PM_i(PM_iminus1);

    // Get dimensions to iterate over all elements
    const auto& dims = PM_i.dimensions();
    std::vector<size_t> indices(dims.size(), 0);

    if (approx_minstar) {
        while (true) {
            // Access current elements
            const T L_val = L_i(indices);

            // Compute flag condition
            const T s = sign(L_val);
            const T val = 0.5 * (1.0 - s);

            // Update PM_i if condition met
            if (val != u_i)
                PM_i(indices) += std::abs(L_val);

            // Increment indices in row-major order
            bool done = true;
            for (int i = dims.size() - 1; i >= 0; --i) {
                if (indices[i] + 1 < dims[i]) {
                    ++indices[i];
                    done = false;
                    break;
                }
                else
                    indices[i] = 0;
            }
            if (done)
                break;
        }
    }
    else {
        while (true) {
            // Access current elements
            const T L_val = L_i(indices);

            // Compute log(1 + exp(-(1 - 2u_i)L_i))
            const T exponent = -(1.0 - 2.0 * u_i) * L_val;
            PM_i(indices) += std::log(1.0 + std::exp(exponent));

            // Increment indices in row-major order
            bool done = true;
            for (int i = dims.size() - 1; i >= 0; --i) {
                if (indices[i] + 1 < dims[i]) {
                    ++indices[i];
                    done = false;
                    break;
                }
                else
                    indices[i] = 0;
            }
            if (done)
                break;
        }
    }

    return PM_i;
}

} // namespace eccpp

#endif // ECCPP_PHI_H
