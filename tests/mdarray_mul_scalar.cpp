#include <gtest/gtest.h>

#include "mdarray.h"

class MdarrayMulScalarTest : public ::testing::Test {
protected:
    eccpp::mdarray<int> createMatrixA() {
        eccpp::mdarray<int> A({4, 3});  // 4 rows x 3 columns
        A({0, 0}) = -5; A({0, 1}) = 10; A({0, 2}) = 0;
        A({1, 0}) = 10; A({1, 1}) = 11; A({1, 2}) = 1;
        A({2, 0}) = -8; A({2, 1}) = 21; A({2, 2}) = 0;
        A({3, 0}) = 33; A({3, 1}) = -1; A({3, 2}) = 2;
        return A;
    }

    eccpp::mdarray<int> createMatrixAmul2() {
        eccpp::mdarray<int> A({4, 3});
        A({0, 0}) = -10; A({0, 1}) = 20; A({0, 2}) = 0;
        A({1, 0}) = 20; A({1, 1}) = 22; A({1, 2}) = 2;
        A({2, 0}) = -16; A({2, 1}) = 42; A({2, 2}) = 0;
        A({3, 0}) = 66; A({3, 1}) = -2; A({3, 2}) = 4;
        return A;
    }

    eccpp::mdarray<int> createMatrixAdiv5() {
        eccpp::mdarray<int> A({4, 3});
        A({0, 0}) = -1; A({0, 1}) = 2; A({0, 2}) = 0;
        A({1, 0}) = 2; A({1, 1}) = 2; A({1, 2}) = 0;
        A({2, 0}) = -1; A({2, 1}) = 4; A({2, 2}) = 0;
        A({3, 0}) = 6; A({3, 1}) = 0; A({3, 2}) = 0;
        return A;
    }
};

TEST_F(MdarrayMulScalarTest, ScalarMultiplication) {
    eccpp::mdarray<int> A = createMatrixA();
    eccpp::mdarray<int> B = createMatrixAmul2();

    eccpp::mdarray<int> C = A * 2;
    EXPECT_EQ(C, B);

    C = 2 * A;
    EXPECT_EQ(C, B);

    C = A;
    C *= 2;
    EXPECT_EQ(C, B);
}

TEST_F(MdarrayMulScalarTest, ScalarDivision) {
    eccpp::mdarray<int> A = createMatrixA();
    eccpp::mdarray<int> B = createMatrixAdiv5();

    eccpp::mdarray<int> C = A / 5;
    EXPECT_EQ(C, B);

    C = A;
    C /= 5;
    EXPECT_EQ(C, B);
}
