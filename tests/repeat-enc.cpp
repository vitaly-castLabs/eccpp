#include <gtest/gtest.h>

#include "repeat-enc.h"

TEST(RepeatEncTest, EncodeBasic) {
    eccpp::repeat_bit_enc rb_enc(3);

    std::vector<int> data = {0, 0, 1};
    auto result = rb_enc.encode(data);
    EXPECT_EQ(result, std::vector<int>({0, 0, 0, 0, 0, 0, 1, 1, 1}));

    eccpp::repeat_msg_enc rm_enc(3);
    result = rm_enc.encode(data);
    EXPECT_EQ(result, std::vector<int>({0, 0, 1, 0, 0, 1, 0, 0, 1}));

    // empty data
    std::vector<int> empty_data;
    result = rb_enc.encode(empty_data);
    EXPECT_EQ(result, empty_data);

    result = rm_enc.encode(empty_data);
    EXPECT_EQ(result, empty_data);

    // r == 0
    rb_enc = eccpp::repeat_bit_enc(0);
    result = rb_enc.encode(data);
    EXPECT_EQ(result, empty_data);

    rm_enc = eccpp::repeat_msg_enc(0);
    result = rm_enc.encode(data);
    EXPECT_EQ(result, empty_data);
}
