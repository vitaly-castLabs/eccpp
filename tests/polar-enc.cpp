#include <gtest/gtest.h>

#include "polar-enc.h"

TEST(PolarEncTest, EncodeBasic4) {
    eccpp::polar_enc enc(4);

    // 0000 -> 0000
    std::vector<int> data = {0, 0, 0, 0};
    auto result = enc.encode(data);
    EXPECT_EQ(result, data);

    // 1111 -> 0001
    data[0] = data[1] = data[2] = data[3] = 1;
    result = enc.encode(data);
    EXPECT_EQ(result, std::vector<int>({0, 0, 0, 1}));

    // 0110 -> 0110
    data[0] = data[3] = 0;
    result = enc.encode(data);
    EXPECT_EQ(result, data);
}

TEST(PolarEncTest, EncodeBasic8) {
    eccpp::polar_enc enc(8);

    std::vector<int> data({0, 0, 0, 0, 0, 0, 0, 0});
    auto result = enc.encode(data);
    EXPECT_EQ(result, data);

    data = {1, 1, 1, 1, 1, 1, 1, 1};
    result = enc.encode(data);
    std::vector<int> expected({0, 0, 0, 0, 0, 0, 0, 1});
    EXPECT_EQ(result, expected);

    data = {1, 1, 0, 1, 0, 1, 0, 1};
    result = enc.encode(data);
    expected = {1, 0, 0, 0, 0, 0, 1, 1};
    EXPECT_EQ(result, expected);
}

TEST(PolarEncTest, EncodeDimensionMismatchThrows) {
    EXPECT_THROW(eccpp::polar_enc(3), std::invalid_argument);
    EXPECT_THROW(eccpp::polar_enc_butterfly(5), std::invalid_argument);

    std::vector<int> data(5);

    eccpp::polar_enc enc(4);
    EXPECT_THROW(enc.encode(data), std::invalid_argument);

    eccpp::polar_enc_butterfly enc1(4);
    EXPECT_THROW(enc1.encode(data), std::invalid_argument);
}

TEST(PolarEncTest, GnMatchesButterfly) {
    eccpp::polar_enc enc(64);
    eccpp::polar_enc_butterfly enc_bfly(64);

    std::vector<int> data(64);
    for (size_t i = 0; i < 64; ++i)
        data[i] = i & 1;

    EXPECT_EQ(enc.encode(data), enc_bfly.encode(data));

    // polar encoding happens to be its own inverse, so double encoding
    // is equivalent to encoding and decoding -> identity transform
    for (size_t i = 0; i < 64; ++i)
        data[i] = (i / 8) & 1;

    EXPECT_EQ(data, enc_bfly.encode(enc.encode(data)));
    EXPECT_EQ(data, enc.encode(enc_bfly.encode(data)));
}
