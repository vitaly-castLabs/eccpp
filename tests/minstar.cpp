#include <gtest/gtest.h>
#include <cmath>
#include <limits>

#include "minstar.h"

class MinStarTest : public ::testing::Test {
protected:
    const double epsilon = 1e-10;
};

TEST_F(MinStarTest, ApproximateMode) {
    // Test approximate mode with positive values
    EXPECT_EQ(2.0, eccpp::minstar(2.0, 3.0, true));
    EXPECT_EQ(1.0, eccpp::minstar(1.0, 5.0, true));

    // Test approximate mode with negative values
    EXPECT_EQ(1.0, eccpp::minstar(-1.0, -5.0, true));
    EXPECT_EQ(2.0, eccpp::minstar(-2.0, -3.0, true));

    // Test approximate mode with mixed signs
    EXPECT_EQ(-1.0, eccpp::minstar(1.0, -1.0, true));
    EXPECT_EQ(-2.0, eccpp::minstar(-2.0, 5.0, true));
}

TEST_F(MinStarTest, ExactMode) {
    // Test exact mode with small values where approximation would be close
    double exact = eccpp::minstar(0.1, 0.2, false);
    double approx = eccpp::minstar(0.1, 0.2, true);
    EXPECT_NE(exact, approx);

    // Expected value using the formula: 2*atanh(tanh(a/2)*tanh(b/2))
    double expected = 2 * std::atanh(std::tanh(0.1/2) * std::tanh(0.2/2));
    EXPECT_NEAR(expected, exact, epsilon);

    // Another test case with different values
    double exact2 = eccpp::minstar(1.0, 0.5, false);
    double expected2 = 2 * std::atanh(std::tanh(1.0/2) * std::tanh(0.5/2));
    EXPECT_NEAR(expected2, exact2, epsilon);
}

TEST_F(MinStarTest, InfinityHandling) {
    double inf = std::numeric_limits<double>::infinity();

    // When one value is infinity, should use the approximate formula
    EXPECT_EQ(2.0, eccpp::minstar(2.0, inf, false));
    EXPECT_EQ(2.0, eccpp::minstar(inf, 2.0, false));

    // Negative infinity
    EXPECT_EQ(2.0, eccpp::minstar(-2.0, -inf, false));
    EXPECT_EQ(2.0, eccpp::minstar(-inf, -2.0, false));

    // Mixed signs with infinity
    EXPECT_EQ(-2.0, eccpp::minstar(2.0, -inf, false));
    EXPECT_EQ(-2.0, eccpp::minstar(-2.0, inf, false));
}

TEST_F(MinStarTest, ZeroHandling) {
    // Test with zeros
    EXPECT_EQ(0.0, eccpp::minstar(0.0, 0.0, true));

    // Test negative zero
    double neg_zero = -0.0;
    EXPECT_EQ(-0.0, eccpp::minstar(neg_zero, 0.0, true));
    EXPECT_EQ(-0.0, eccpp::minstar(0.0, neg_zero, true));
    EXPECT_EQ(-0.0, eccpp::minstar(neg_zero, neg_zero, true));

    // Test exact calculation with zeros
    double exact_zero = eccpp::minstar(0.0, 0.0, false);
    EXPECT_NEAR(0.0, exact_zero, epsilon);
}

TEST_F(MinStarTest, EdgeCases) {
    // Very large finite values
    double large_val = 1e100;
    EXPECT_EQ(large_val, eccpp::minstar(large_val, 2 * large_val, true));

    // Very small values
    double small_val = 1e-100;
    EXPECT_EQ(small_val, eccpp::minstar(small_val, 2 * small_val, true));

    // NaN handling (this should propagate NaN)
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(eccpp::minstar(nan_val, 1.0, true)));
    EXPECT_TRUE(std::isnan(eccpp::minstar(1.0, nan_val, true)));
}
