#include <gtest/gtest.h>

#include "polar-enc.h"

TEST(PolarEncTest, EncodeBasic4) {
    eccpp::polar_enc enc(4);

    eccpp::mdarray<int> data({4});
    data({0}) = 0;
    data({1}) = 0;
    data({2}) = 0;
    data({3}) = 0;
    auto result = enc.encode(data);
    eccpp::mdarray<int> expected({4});
    expected({0}) = 0;
    expected({1}) = 0;
    expected({2}) = 0;
    expected({3}) = 0;
    EXPECT_EQ(result, expected);

    data({0}) = 1;
    data({1}) = 1;
    data({2}) = 1;
    data({3}) = 1;
    result = enc.encode(data);
    expected({0}) = 0;
    expected({1}) = 0;
    expected({2}) = 0;
    expected({3}) = 1;
    EXPECT_EQ(result, expected);

    data({0}) = 0;
    data({1}) = 1;
    data({2}) = 1;
    data({3}) = 0;
    result = enc.encode(data);
    EXPECT_EQ(result, data);    // 0110 conveniently encodes to 0110, just like 0000 -> 0000
}

TEST(PolarEncTest, EncodeBasic8) {
    eccpp::polar_enc enc(8);

    eccpp::mdarray<int> data({8});
    data({0}) = 0;
    data({1}) = 0;
    data({2}) = 0;
    data({3}) = 0;
    data({4}) = 0;
    data({5}) = 0;
    data({6}) = 0;
    data({7}) = 0;
    auto result = enc.encode(data);
    EXPECT_EQ(result, data);

    data({0}) = 1;
    data({1}) = 1;
    data({2}) = 1;
    data({3}) = 1;
    data({4}) = 1;
    data({5}) = 1;
    data({6}) = 1;
    data({7}) = 1;
    result = enc.encode(data);
    eccpp::mdarray<int> expected({8});
    expected({0}) = 0;
    expected({1}) = 0;
    expected({2}) = 0;
    expected({3}) = 0;
    expected({4}) = 0;
    expected({5}) = 0;
    expected({6}) = 0;
    expected({7}) = 1;
    EXPECT_EQ(result, expected);

    data({0}) = 1;
    data({1}) = 1;
    data({2}) = 0;
    data({3}) = 1;
    data({4}) = 0;
    data({5}) = 1;
    data({6}) = 0;
    data({7}) = 1;
    result = enc.encode(data);
    expected({0}) = 1;
    expected({1}) = 0;
    expected({2}) = 0;
    expected({3}) = 0;
    expected({4}) = 0;
    expected({5}) = 0;
    expected({6}) = 1;
    expected({7}) = 1;
    EXPECT_EQ(result, expected);
}

TEST(PolarEncTest, EncodeDimensionMismatchThrows) {
    EXPECT_THROW(eccpp::polar_enc(3), std::invalid_argument);
    EXPECT_THROW(eccpp::polar_enc_butterfly(5), std::invalid_argument);

    eccpp::mdarray<int> data({5});
    eccpp::mdarray<int> data1({4, 4});

    eccpp::polar_enc enc(4);
    EXPECT_THROW(enc.encode(data), std::invalid_argument);
    EXPECT_THROW(enc.encode(data1), std::invalid_argument);

    eccpp::polar_enc_butterfly enc1(4);
    EXPECT_THROW(enc1.encode(data), std::invalid_argument);
    EXPECT_THROW(enc1.encode(data1), std::invalid_argument);
}

TEST(PolarEncTest, GnMatchesButterfly) {
    eccpp::polar_enc enc(64);
    eccpp::polar_enc_butterfly enc_bfly(64);

    eccpp::mdarray<int> data({64});
    for (size_t i = 0; i < 64; ++i)
        data({i}) = i & 1;

    EXPECT_EQ(enc.encode(data), enc_bfly.encode(data));

    // polar encoding happens to be its own inverse, so double encoding
    // is equivalent to encoding and decoding -> identity transform
    for (size_t i = 0; i < 64; ++i)
        data({i}) = (i / 8) & 1;

    EXPECT_EQ(data, enc_bfly.encode(enc.encode(data)));
    EXPECT_EQ(data, enc.encode(enc_bfly.encode(data)));
}
