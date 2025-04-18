#include <gtest/gtest.h>

#include "phi.h"

TEST(PhiTest, ApproxMinStarTrueBasicUpdate) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 5.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 3.0;
    auto u_i = 1.0; // val=0.0 != u_val=1.0 → add 3.0

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
    auto u_i = 0.0; // val=0.0 == u_val=0.0 → no update

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
    auto u_i = 0.0;

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, false);
    double expected_val = std::log(2.0); // log(1 + exp(0.0)) = log(2)

    EXPECT_DOUBLE_EQ(result({0}), expected_val);
}

TEST(PhiTest, ApproxMinStarFalseWithExponent) {
    eccpp::mdarray<double> PM_iminus1({1});
    PM_iminus1({0}) = 2.0;
    eccpp::mdarray<double> L_i({1});
    L_i({0}) = 1.0;
    auto u_i = 1.0; // exponent=1.0 → log(1 + exp(1))

    auto result = eccpp::phi(PM_iminus1, L_i, u_i, false);
    double expected_val = 2.0 + std::log(1.0 + std::exp(1.0));

    EXPECT_DOUBLE_EQ(result({0}), expected_val);
}

TEST(PhiTest, DimensionMismatchThrows) {
    eccpp::mdarray<double> PM_iminus1({2});
    eccpp::mdarray<double> L_i({3});
    auto u_i = 0.0;

    EXPECT_THROW(eccpp::phi(PM_iminus1, L_i, u_i, true), std::invalid_argument);
}

TEST(PhiTest, Polar5G) {
    // Define 3D dimensions {1, 1, 8}
    std::vector<size_t> dims = {1, 1, 8};

    eccpp::mdarray<double> PM_iminus1(dims);
    std::vector<double> pm_values = {0, 10, 20, 20, 30, 40, 40, 60};
    for (size_t k = 0; k < 8; ++k)
        PM_iminus1({0, 0, k}) = pm_values[k];

    eccpp::mdarray<double> L_i(dims);
    std::vector<double> l_values = {250, 230, 210, 210, 190, 170, 170, -130};
    for (size_t k = 0; k < 8; ++k)
        L_i({0, 0, k}) = l_values[k];

    auto u_i = 1.0;

    // Call phi with approx_minstar = true
    auto result = eccpp::phi(PM_iminus1, L_i, u_i, true);

    // Expected PM_i after update
    eccpp::mdarray<double> expected(dims);
    std::vector<double> expected_values = {250, 240, 230, 230, 220, 210, 210, 60};
    for (size_t k = 0; k < 8; ++k)
        expected({0, 0, k}) = expected_values[k];

    // Verify equality
    EXPECT_EQ(result, expected);
}
