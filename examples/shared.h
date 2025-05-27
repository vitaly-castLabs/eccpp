#ifndef ECCPP_EXAMPLES_SHARED_H
#define ECCPP_EXAMPLES_SHARED_H

#include <vector>

static std::vector<float> bits_to_llr(const std::vector<int>& bits, float confidence = 10) {
    std::vector<float> llr(bits.size());
    for (size_t i = 0; i < bits.size(); ++i) {
        const auto bit = bits[i];
        // negative (-1 usually) value is utilized to represent erasure / unknown bit
        llr[i] = bit < 0 ? 0 : (bit ? -confidence : confidence);
    }
    return llr;
}

#endif // ECCPP_EXAMPLES_SHARED_H
