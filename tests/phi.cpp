#include <gtest/gtest.h>

#include "phi.h"

TEST(PhiTest, ApproxMinStarTrueBasicUpdate) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 5.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 3.0;
    eccpp::mdarray<double> u_i({1});
    u_i({0}) = 1.0; // val=0.0 != u_val=1.0 → add 3.0

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, true);
    eccpp::mdarray<double> expected({1});
    expected({0}) = 8.0;

    EXPECT_EQ(result, expected);
}

TEST(PhiTest, ApproxMinStarTrueNoUpdate) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 5.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 3.0;
    eccpp::mdarray<double> u_i({1});
    u_i({0}) = 0.0; // val=0.0 == u_val=0.0 → no update

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, true);
    eccpp::mdarray<double> expected({1});
    expected({0}) = 5.0;

    EXPECT_EQ(result, expected);
}

TEST(PhiTest, ApproxMinStarFalseBasic) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 0.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 0.0;
    eccpp::mdarray<double> u_i({1});
    u_i({0}) = 0.0;

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, false);
    double expected_val = std::log(2.0); // log(1 + exp(0.0)) = log(2)

    EXPECT_DOUBLE_EQ(result({0}), expected_val);
}

TEST(PhiTest, ApproxMinStarFalseWithExponent) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 2.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 1.0;
    eccpp::mdarray<double> u_i({1});
    u_i({0}) = 1.0; // exponent=1.0 → log(1 + exp(1))

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, false);
    double expected_val = 2.0 + std::log(1.0 + std::exp(1.0));

    EXPECT_DOUBLE_EQ(result({0}), expected_val);
}

TEST(PhiTest, DimensionMismatchThrows) {
    eccpp::mdarray<double> PM({2});
    PM({0}) = 1.0; PM({1}) = 2.0;
    eccpp::mdarray<double> L({3});
    L({0}) = 1.0; L({1}) = 2.0; L({2}) = 3.0;
    eccpp::mdarray<double> u({2});
    u({0}) = 0.0; u({1}) = 0.0;

    EXPECT_THROW(eccpp::phi(PM, L, u, true), std::invalid_argument);
}

TEST(PhiTest, ApproxMinStarTrueMultiElement) {
    eccpp::mdarray<double> PM_iminus1({2, 2});
    PM_iminus1({0, 0}) = 2.0; PM_iminus1({0, 1}) = 3.0;
    PM_iminus1({1, 0}) = 4.0; PM_iminus1({1, 1}) = 5.0;

    eccpp::mdarray<double> L_i({2, 2});
    L_i({0, 0}) = -1.0; L_i({0, 1}) = 2.0;
    L_i({1, 0}) = 3.0; L_i({1, 1}) = -4.0;

    eccpp::mdarray<double> u_i({2, 2});
    u_i({0, 0}) = 0.0; u_i({0, 1}) = 1.0;
    u_i({1, 0}) = 1.0; u_i({1, 1}) = 0.0;

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, true);
    eccpp::mdarray<double> expected({2, 2});
    expected({0, 0}) = 3.0; // 2 + |-1| = 3
    expected({0, 1}) = 5.0; // 3 + |2| = 5
    expected({1, 0}) = 7.0; // 4 + |3| = 7
    expected({1, 1}) = 9.0; // 5 + |-4| = 9

    EXPECT_EQ(result, expected);
}
