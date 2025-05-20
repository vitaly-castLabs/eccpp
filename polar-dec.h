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
    polar_dec(size_t n, std::uint_fast32_t permutation_seed = 0) : n_(n), permutation_seed_(permutation_seed) {
        if (!n || (n & (n - 1)) != 0)
            throw std::invalid_argument("n must be a power of 2");
    }

    struct result {
        std::vector<int> msg;
        T confidence = 0;
    };

    // llr is a vector of Log-Likelihood Ratios for soft decoding: log(p(0)/p(1)). I.e. a negative LLR
    // value means that the bit is more likely to be 1, while a positive value means that the bit is
    // more likely to be 0. Zero LLR stands for "no idea" / "erasure" / "the bit was punctured" / etc.
    result decode(const std::vector<T>& llr, const std::vector<size_t>& info_bits) const {
        if (llr.size() != n_)
            throw std::invalid_argument("LLR size must match transform size");

        if (info_bits.empty())
            throw std::invalid_argument("Info bits must not be empty");

        if (info_bits.size() > n_)
            throw std::invalid_argument("Info bits size greater than transform size");

        result dec_result;
        dec_result.msg.resize(info_bits.size());

        // encode all possible messages and pick the best match
        T best_match = std::numeric_limits<T>::lowest();
        T second_best_match = best_match;
        polar_enc_butterfly enc(n_, permutation_seed_);
        std::vector<int> msg_with_frozen_bits(n_);
        while (true) {
            const auto codeword = enc.encode(msg_with_frozen_bits);
            T match = 0;
            for (size_t i = 0; i < n_; ++i)
                match += codeword[i] ? -llr[i] : llr[i];

            if (match > best_match) {
                second_best_match = best_match;
                best_match = match;
                extract_message(dec_result.msg, msg_with_frozen_bits, info_bits);
            }
            else if (match > second_best_match)
                second_best_match = match;

            if (!next_message(msg_with_frozen_bits, info_bits))
                break;
        }

        T llr_sum = 0;
        for (auto v: llr)
            llr_sum += std::abs(v);

        // ideally the best match should be close to sum(abs(llr)) and the runner-up should be
        // significantly lower - if both are true, then we can be confident we didn't pick up
        // a random value
        dec_result.confidence = best_match * (best_match - second_best_match) / (llr_sum * llr_sum);
        return dec_result;
    }

    // same as before, but we have only partial LLR array and we don't know the offset/position,
    // so we need to conduct correlation search as well
    result decode_unaligned(const std::vector<T>& llr, const std::vector<size_t>& info_bits) const {
        if (llr.size() > n_)
            throw std::invalid_argument("LLR size must not exceed transform size");

        if (info_bits.empty())
            throw std::invalid_argument("Info bits must not be empty");

        if (info_bits.size() > n_)
            throw std::invalid_argument("Info bits size greater than transform size");

        result dec_result;
        dec_result.msg.resize(info_bits.size());

        // encode all possible messages and pick the best match
        T best_match = std::numeric_limits<T>::lowest();
        T second_best_match = best_match;
        polar_enc_butterfly enc(n_, permutation_seed_);
        std::vector<int> msg_with_frozen_bits(n_);
        const auto max_offset = n_ - llr.size();
        while (true) {
            const auto codeword = enc.encode(msg_with_frozen_bits);
            for (auto off = 0; off <= max_offset; ++off) {
                T match = 0;
                for (size_t i = 0; i < llr.size(); ++i)
                    match += codeword[i + off] ? -llr[i] : llr[i];

                if (match > best_match) {
                    second_best_match = best_match;
                    best_match = match;
                    extract_message(dec_result.msg, msg_with_frozen_bits, info_bits);
                }
                else if (match > second_best_match)
                    second_best_match = match;
            }

            if (!next_message(msg_with_frozen_bits, info_bits))
                break;
        }

        T llr_sum = 0;
        for (auto v: llr)
            llr_sum += std::abs(v);

        // ideally the best match should be close to sum(abs(llr)) and the runner-up should be
        // significantly lower - if both are true, then we can be confident we didn't pick up
        // a random value
        dec_result.confidence = best_match * (best_match - second_best_match) / (llr_sum * llr_sum);
        return dec_result;
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
    const std::uint_fast32_t permutation_seed_;
};

} // namespace eccpp

#endif // ECCPP_POLAR_DEC_H
