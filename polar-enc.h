#ifndef ECCPP_POLAR_ENC_H
#define ECCPP_POLAR_ENC_H

#include <vector>

#include "gn.h"
#include "shuffle.h"

namespace eccpp {

class polar_enc {
public:
    // permutation_seed is used to shuffle the generator matrix rows before encoding
    // (or shuffle the bits of the codeword after encoding, which is equivalent).
    // permutation_seed = 0 means no shuffling, sometimes refered as "natural order".
    // If you come across "bit-reverse permutation matrix BN", it's the same thing.
    polar_enc(size_t n, std::uint_fast32_t permutation_seed = 0) : gn_(gn(n)), permutation_seed_(permutation_seed) {}

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

        if (permutation_seed_)
            eccpp::shuffle(result, permutation_seed_);

        return result;
    }

private:
    const mdarray<int> gn_;
    const std::uint_fast32_t permutation_seed_;
};

//
// same thing as polar_enc, but uses butterfly polar transform instead of generator matrix.
// Tons time faster than G_n multiplication for large N.
//
class polar_enc_butterfly {
public:
    polar_enc_butterfly(size_t n, std::uint_fast32_t permutation_seed = 0) : N(n), permutation_seed_(permutation_seed) {
        if (!n || (n & (n - 1)) != 0)
            throw std::invalid_argument("n must be a power of 2");
    }

    std::vector<int> encode(const std::vector<int>& data) const {
        if (N != data.size())
            throw std::invalid_argument("Data size must match transform size");

        auto result = data;
        for (size_t step = 1; step < N; step *= 2) {
            for (size_t i = 0; i < N; i += step * 2) {
                for (size_t j = 0; j < step; ++j) {
                    int a = result[i + j];
                    int b = result[i + j + step];
                    result[i + j] = a ^ b;
                    result[i + j + step] = b;
                }
            }
        }

        if (permutation_seed_)
            eccpp::shuffle(result, permutation_seed_);

        return result;
    }

private:
    const size_t N;
    const std::uint_fast32_t permutation_seed_;
};

} // namespace eccpp

#endif // ECCPP_POLAR_ENC_H
