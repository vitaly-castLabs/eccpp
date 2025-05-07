#include <gtest/gtest.h>

#include "polar-dec.h"

using T = float;

std::vector<T> bits_to_llr(const std::vector<int>& bits) {
    std::vector<T> llr(bits.size());
    for (size_t i = 0; i < bits.size(); ++i) {
        const auto bit = bits[i];
        // negative (-1 usually) value is utilized to represent erasure / unknown bit
        llr[i] = bit < 0 ? 0 : (bit ? -10 : 10);
    }
    return llr;
}


TEST(PolarDecTest, ThrowOnWrongInput) {
    EXPECT_THROW(eccpp::polar_dec<T>(0), std::invalid_argument);
    EXPECT_THROW(eccpp::polar_dec<T>(3), std::invalid_argument);
    EXPECT_THROW(eccpp::polar_dec<T>(5), std::invalid_argument);

    eccpp::polar_dec<T> dec(4);
    EXPECT_THROW(dec.decode(bits_to_llr({1, 2, 3}), {0, 1}), std::invalid_argument);
    EXPECT_THROW(dec.decode(bits_to_llr({0, 0, 0, 0}), {}), std::invalid_argument);
    EXPECT_THROW(dec.decode(bits_to_llr({0, 0, 0, 0}), {0, 1, 2, 3, 4}), std::invalid_argument);
}

TEST(PolarDecTest, DecodeBasic4) {
    eccpp::polar_dec<T> dec(4);
    const std::vector<size_t> info_bits({0, 1, 2, 3});

    // 0000 -> polar_enc -> 0000 -> polar_dec -> 0000
    std::vector<int> cw(4);
    auto msg = cw; // happen to be the same
    auto result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);

    // 1111 -> polar_enc -> 0001 -> polar_dec -> 1111
    cw = {0, 0, 0, 1};
    msg = {1, 1, 1, 1};
    result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);

    // 0110 -> 0110
    cw = {0, 1, 1, 0};
    msg = cw;
    result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);
}

TEST(PolarDecTest, DecodeBasic8) {
    eccpp::polar_dec<T> dec(8);
    const std::vector<size_t> info_bits({0, 1, 2, 3, 4, 5, 6, 7});

    // 00000000 -> polar_enc -> 00000000 -> polar_dec -> 00000000
    std::vector<int> cw(8);
    auto msg = cw;
    auto result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);

    // 11111111 -> 00000001
    cw = {0, 0, 0, 0, 0, 0, 0, 1};
    msg = {1, 1, 1, 1, 1, 1, 1, 1};
    result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);

    // 11010101 -> 10000011
    cw = {1, 0, 0, 0, 0, 0, 1, 1};
    msg = {1, 1, 0, 1, 0, 1, 0, 1};
    result = dec.decode(bits_to_llr(cw), info_bits);
    EXPECT_EQ(result, msg);
}

TEST(PolarDecTest, FrozenBits) {
    eccpp::polar_enc_butterfly enc(32);
    eccpp::polar_dec<T> dec(32);

    std::vector<int> msg_with_frozen_bits(32);
    std::vector<int> msg = {1, 0, 1, 0, 1, 0};
    std::vector<size_t> info_bits = {15, 23, 27, 29, 30, 31};
    for (auto i = 0; i < info_bits.size(); ++i)
        msg_with_frozen_bits[info_bits[i]] = msg[i];

    auto cw = enc.encode(msg_with_frozen_bits);
    auto llr = bits_to_llr(cw);
    // simulate some erasures
    llr[1] = llr[2] = llr[4] = llr[8] = llr[16] = 0;
    auto result = dec.decode(llr, info_bits);
    EXPECT_EQ(result, msg);

    // add a few bit flips
    llr[0] = -llr[0], llr[7] = -llr[7], llr[31] = -llr[31];
    result = dec.decode(llr, info_bits);
    EXPECT_EQ(result, msg);
}
