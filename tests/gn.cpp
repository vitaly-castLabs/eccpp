#include <gtest/gtest.h>

#include "gn.h"

TEST(GnTest, N1) {
    eccpp::mdarray<int> expected({1});
    expected({0}) = 1;
    auto result = eccpp::gn(1);

    EXPECT_EQ(result, expected);
}

TEST(GnTest, N2) {
    eccpp::mdarray<int> expected({2, 2});
    expected({0, 0}) = 1;
    expected({0, 1}) = 0;
    expected({1, 0}) = 1;
    expected({1, 1}) = 1;
    auto result = eccpp::gn(2);

    EXPECT_EQ(result, expected);
}

TEST(GnTest, N3) {
    EXPECT_THROW(eccpp::gn(3), std::invalid_argument);
}

TEST(GnTest, N4) {
    eccpp::mdarray<int> expected({4, 4});
    expected({0, 0}) = 1; expected({0, 1}) = 0; expected({0, 2}) = 0; expected({0, 3}) = 0;
    expected({1, 0}) = 1; expected({1, 1}) = 1; expected({1, 2}) = 0; expected({1, 3}) = 0;
    expected({2, 0}) = 1; expected({2, 1}) = 0; expected({2, 2}) = 1; expected({2, 3}) = 0;
    expected({3, 0}) = 1; expected({3, 1}) = 1; expected({3, 2}) = 1; expected({3, 3}) = 1;
    auto result = eccpp::gn(4);

    EXPECT_EQ(result, expected);
}