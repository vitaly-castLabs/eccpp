#ifndef ECCPP_POLAR_ENC_H
#define ECCPP_POLAR_ENC_H

#include <cmath>
#include <vector>

#include "gn.h"

namespace eccpp {

class polar_enc {
public:
    polar_enc(size_t n) : gn_(gn(n)) {}

    std::vector<int> encode(const std::vector<int>& data) const {
        const auto N = data.size();
        if (N != gn_.dimensions()[0])
            throw std::invalid_argument("Data size must match generator matrix size");

        // this won't work as we need GF(2), not regular arithmetics
        //return data * gn_;

        std::vector<int> result(N);
        for (size_t i = 0; i < N; ++i) {
            int r = 0;
            for (size_t j = 0; j < N; ++j)
                r ^= data[j] & gn_({j, i});

            result[i] = r;
        }
        return result;
    }

private:
    const mdarray<int> gn_;
};

//
// same thing as polar_enc, but uses butterfly polar transform instead of generator matrix.
// Tons time faster than G_n multiplication for large N.
//
class polar_enc_butterfly {
public:
    polar_enc_butterfly(size_t n) : N(n), log2N(static_cast<size_t>(std::log2(n) + 0.5)) {
        if (!n || (n & (n - 1)) != 0)
            throw std::invalid_argument("n must be a power of 2");
    }

    std::vector<int> encode(const std::vector<int>& data) const {
        if (N != data.size())
            throw std::invalid_argument("Data size must match transform size");

        auto result = data;
        for (size_t stage = 0; stage < log2N; ++stage) {
            const size_t step = 1 << stage;
            for (size_t i = 0; i < N; i += 2 * step) {
                for (size_t j = 0; j < step; ++j) {
                    int a = result[i + j];
                    int b = result[i + j + step];
                    result[i + j] = a ^ b;
                    result[i + j + step] = b;
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
