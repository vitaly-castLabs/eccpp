#ifndef ECCPP_POLAR_ENC_H
#define ECCPP_POLAR_ENC_H

#include <vector>
#include <cmath>

#include "gn.h"

namespace eccpp {

class polar_enc {
public:
    polar_enc(size_t n) : gn_(gn(n)) {}

    mdarray<int> encode(const mdarray<int>& data) const {
        if (data.dimensions().size() != 1)
            throw std::invalid_argument("Data must be a vector");

        const auto N = data.dimensions()[0];
        if (N != gn_.dimensions()[0])
            throw std::invalid_argument("Data size must match generator matrix size");

        // this won't work as we need GF(2), not regular arithmetics
        //return data * gn_;

        mdarray<int> result({N});
        for (size_t i = 0; i < N; ++i) {
            int r = 0;
            for (size_t j = 0; j < N; ++j)
                r ^= data({j}) & gn_({j, i});

            result({i}) = r;
        }
        return result;
    }

private:
    const mdarray<int> gn_;
};

//
// same thing as polar_enc, but uses butterfly polar transform instead of generator matrix.
// I haven't measured performance yet, but I expect this to be faster.
//
class polar_enc_butterfly {
public:
    polar_enc_butterfly(size_t n) : N(n), log2N(static_cast<size_t>(std::log2(n) + 0.5)) {
        if (!n || (n & (n - 1)) != 0)
            throw std::invalid_argument("n must be a power of 2");
    }

    mdarray<int> encode(const mdarray<int>& data) const {
        if (data.dimensions().size() != 1)
            throw std::invalid_argument("Data must be a vector");

        if (N != data.dimensions()[0])
            throw std::invalid_argument("Data size must match transform size");

        auto result = data;
        for (size_t stage = 0; stage < log2N; ++stage) {
            size_t step = 1 << stage;
            for (size_t i = 0; i < N; i += 2 * step) {
                for (size_t j = 0; j < step; ++j) {
                    int a = result({i + j});
                    int b = result({i + j + step});
                    result({i + j}) = a ^ b;
                    result({i + j + step}) = b;
                }
            }
        }
        return result;
    }

private:
    const size_t N;
    const size_t log2N;
};

} // namespace eccpp

#endif // ECCPP_POLAR_ENC_H
