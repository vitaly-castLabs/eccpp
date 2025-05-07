#ifndef ECCPP_POLAR_DEC_H
#define ECCPP_POLAR_DEC_H

#include <vector>
#include <limits>

#include "polar-enc.h"

namespace eccpp {

// brute-force ML (maximum likelihood) decoder
template <typename T>
class polar_dec {
public:
    polar_dec(size_t n) : n_(n) {
        if (!n || (n & (n - 1)) != 0)
            throw std::invalid_argument("n must be a power of 2");
    }

    // llr is a vector of Log-Likelihood Ratios for soft decoding: log(p(0)/p(1)). I.e. a negative LLR
    // value means that the bit is more likely to be 1, while a positive value means that the bit is
    // more likely to be 0. Zero LLR stands for "no idea" / "erasure" / "the bit was punctured" / etc.
    std::vector<int> decode(const std::vector<T>& llr, const std::vector<size_t>& info_bits) const {
        if (llr.size() != n_)
            throw std::invalid_argument("LLR size must match transform size");

        if (info_bits.empty())
            throw std::invalid_argument("Info bits must not be empty");

        if (info_bits.size() > n_)
            throw std::invalid_argument("Info bits size greater than transform size");

        std::vector<int> result(info_bits.size());
        T best_match = std::numeric_limits<T>::lowest();

        // encode all possible messages and pick the best match
        polar_enc_butterfly enc(n_);
        std::vector<int> msg_with_frozen_bits(n_);
        while (true) {
            const auto codeword = enc.encode(msg_with_frozen_bits);
            T match = 0;
            for (size_t i = 0; i < n_; ++i)
                match += codeword[i] ? -llr[i] : llr[i];

            if (match > best_match) {
                best_match = match;
                extract_message(result, msg_with_frozen_bits, info_bits);
            }

            if (!next_message(msg_with_frozen_bits, info_bits))
                break;
        }

        return result;
    }

private:
    // increment message bits (f are frozen bits): ff0f0 -> ff0f1 -> ff1f0 -> ff1f1. Returns
    // false (no more messages) at 11..1 -> 00..0 roll-over.
    static bool next_message(std::vector<int>& msg_with_frozen_bits, const std::vector<size_t>& info_bits) {
        for (size_t i = 0; i < info_bits.size(); ++i) {
            auto& bit = msg_with_frozen_bits[info_bits[i]];
            bit ^= 1;

            if (bit) {
                for (size_t j = 0; j < i; ++j)
                    msg_with_frozen_bits[info_bits[j]] = 0;

                return true;
            }
        }
        return false;
    }

    static void extract_message(std::vector<int>& msg, const std::vector<int>& msg_with_frozen_bits, const std::vector<size_t>& info_bits) {
        for (size_t i = 0; i < info_bits.size(); ++i)
            msg[i] = msg_with_frozen_bits[info_bits[i]];
    }

    const size_t n_;
};

} // namespace eccpp

#endif // ECCPP_POLAR_DEC_H
