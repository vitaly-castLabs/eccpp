#include <gtest/gtest.h>

#include "repeat-enc.h"

TEST(RepeatEncTest, EncodeBasic) {
    eccpp::repeat_bit_enc rb_enc(3);

    eccpp::mdarray<int> data({3});
    data({0}) = 0;
    data({1}) = 0;
    data({2}) = 1;
    auto result = rb_enc.encode(data);
    eccpp::mdarray<int> expected({9});
    expected({0}) = 0;
    expected({1}) = 0;
    expected({2}) = 0;
    expected({3}) = 0;
    expected({4}) = 0;
    expected({5}) = 0;
    expected({6}) = 1;
    expected({7}) = 1;
    expected({8}) = 1;
    EXPECT_EQ(result, expected);

    eccpp::repeat_msg_enc rm_enc(3);
    result = rm_enc.encode(data);
    expected({0}) = 0;
    expected({1}) = 0;
    expected({2}) = 1;
    expected({3}) = 0;
    expected({4}) = 0;
    expected({5}) = 1;
    expected({6}) = 0;
    expected({7}) = 0;
    expected({8}) = 1;
    EXPECT_EQ(result, expected);
}

TEST(RepeatEncTest, WrongParamHandling) {
    eccpp::repeat_bit_enc rb_enc(3);
    eccpp::repeat_msg_enc rm_enc(3);

    eccpp::mdarray<int> data({4, 4});
    EXPECT_THROW(rb_enc.encode(data), std::invalid_argument);
    EXPECT_THROW(rm_enc.encode(data), std::invalid_argument);

    EXPECT_THROW(eccpp::repeat_bit_enc(0), std::invalid_argument);
    EXPECT_THROW(eccpp::repeat_msg_enc(0), std::invalid_argument);
}
