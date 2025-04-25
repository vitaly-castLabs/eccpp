#include <gtest/gtest.h>

#include "hamdist.h"

TEST(HamdistTest, Basic) {
    eccpp::mdarray<int> a({2, 2});
    a({0, 0}) = 0;
    a({0, 1}) = 1;
    a({1, 0}) = 1;
    a({1, 1}) = 0;

    eccpp::mdarray<int> b({2, 2});
    b({0, 0}) = 0;
    b({0, 1}) = 1;
    b({1, 0}) = 1;
    b({1, 1}) = 0;

    EXPECT_EQ(eccpp::hamdist(a, b), 0);
    EXPECT_EQ(eccpp::hamdist(b, a), 0);

    b({0, 0}) ^= 1;
    EXPECT_EQ(eccpp::hamdist(a, b), 1);
    EXPECT_EQ(eccpp::hamdist(b, a), 1);

    a({1, 1}) ^= 1;
    EXPECT_EQ(eccpp::hamdist(a, b), 2);
    EXPECT_EQ(eccpp::hamdist(b, a), 2);

    eccpp::mdarray<int> c({5});
    c({0}) = 1;
    c({1}) = 1;
    c({2}) = 1;
    c({3}) = 1;
    c({4}) = 1;

    eccpp::mdarray<int> d({5});
    d({0}) = 0;
    d({1}) = 0;
    d({2}) = 0;
    d({3}) = 0;
    d({4}) = 0;
    EXPECT_EQ(eccpp::hamdist(c, d), 5);
    EXPECT_EQ(eccpp::hamdist(d, c), 5);

    d({0}) ^= 1;
    d({2}) ^= 1;
    d({4}) ^= 1;
    EXPECT_EQ(eccpp::hamdist(c, d), 2);
    EXPECT_EQ(eccpp::hamdist(d, c), 2);
}

TEST(HamdistTest, DifferentSizes) {
    eccpp::mdarray<int> a({2, 2});
    eccpp::mdarray<int> b({2, 3});
    eccpp::mdarray<int> c({5});
    EXPECT_THROW(eccpp::hamdist(a, b), std::invalid_argument);
    EXPECT_THROW(eccpp::hamdist(a, c), std::invalid_argument);
    EXPECT_THROW(eccpp::hamdist(b, c), std::invalid_argument);
}
