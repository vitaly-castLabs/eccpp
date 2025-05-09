#include <gtest/gtest.h>

#include "longest-common.h"

TEST(LcTest, Basic) {
    std::vector<int> a = {1, 0, 1, 0, 1};
    std::vector<int> b = {1, 1, 1};
    EXPECT_EQ(eccpp::lc_run(a, b), 1);
    EXPECT_EQ(eccpp::lc_run(b, a), 1);
    EXPECT_EQ(eccpp::lc_subseq(a, b), 3);
    EXPECT_EQ(eccpp::lc_subseq(b, a), 3);

    b = {1, 8, 1, 8, 1};
    EXPECT_EQ(eccpp::lc_run(a, b), 1);
    EXPECT_EQ(eccpp::lc_subseq(a, b), 3);

    a = {};
    EXPECT_EQ(eccpp::lc_run(a, b), 0);
    EXPECT_EQ(eccpp::lc_run(b, a), 0);
    EXPECT_EQ(eccpp::lc_subseq(a, b), 0);
    EXPECT_EQ(eccpp::lc_subseq(b, a), 0);

    a = {1, 2, 3, 4, 5};
    b = {6};
    EXPECT_EQ(eccpp::lc_run(a, b), 0);
    EXPECT_EQ(eccpp::lc_run(b, a), 0);
    EXPECT_EQ(eccpp::lc_subseq(a, b), 0);
    EXPECT_EQ(eccpp::lc_subseq(b, a), 0);

    a = {-1, 0, -2, -3, 100, 0,-4};
    b = {0, -2, -3, 0, -4, -5};
    EXPECT_EQ(eccpp::lc_run(a, b), 3);
    EXPECT_EQ(eccpp::lc_run(b, a), 3);
    EXPECT_EQ(eccpp::lc_subseq(a, b), 5);
    EXPECT_EQ(eccpp::lc_subseq(b, a), 5);
}
