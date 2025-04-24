#include <gtest/gtest.h>

#include "mdarray.h"

class MdarrayMulTest : public ::testing::Test {
protected:
    // Matrices from the Wikipedia example:
    // https://en.wikipedia.org/wiki/Matrix_multiplication#Resource_allocation_in_economics
    eccpp::mdarray<int> createMatrixA() {
        eccpp::mdarray<int> A({4, 3});  // 4 rows x 3 columns
        A({0, 0}) = 1; A({0, 1}) = 0; A({0, 2}) = 1;
        A({1, 0}) = 2; A({1, 1}) = 1; A({1, 2}) = 1;
        A({2, 0}) = 0; A({2, 1}) = 1; A({2, 2}) = 1;
        A({3, 0}) = 1; A({3, 1}) = 1; A({3, 2}) = 2;
        return A;
    }

    eccpp::mdarray<int> createMatrixB() {
        eccpp::mdarray<int> B({3, 3});  // 3 rows x 3 columns
        B({0, 0}) = 1; B({0, 1}) = 2; B({0, 2}) = 1;
        B({1, 0}) = 2; B({1, 1}) = 3; B({1, 2}) = 1;
        B({2, 0}) = 4; B({2, 1}) = 2; B({2, 2}) = 2;
        return B;
    }

    // The product AB = [5 4 3; 8 9 5; 6 5 3; 11 9 6]
    eccpp::mdarray<int> createExpectedAB() {
        eccpp::mdarray<int> AB({4, 3});  // 4 rows x 3 columns
        AB({0, 0}) = 5; AB({0, 1}) = 4; AB({0, 2}) = 3;
        AB({1, 0}) = 8; AB({1, 1}) = 9; AB({1, 2}) = 5;
        AB({2, 0}) = 6; AB({2, 1}) = 5; AB({2, 2}) = 3;
        AB({3, 0}) = 11; AB({3, 1}) = 9; AB({3, 2}) = 6;
        return AB;
    }

    // Create 3D tensors for testing higher-dimensional Muls
    eccpp::mdarray<int> create3DTensor() {
        eccpp::mdarray<int> tensor({2, 2, 3});
        // First 2x3 slice
        tensor({0, 0, 0}) = 1; tensor({0, 0, 1}) = 2; tensor({0, 0, 2}) = 3;
        tensor({0, 1, 0}) = 4; tensor({0, 1, 1}) = 5; tensor({0, 1, 2}) = 6;
        // Second 2x3 slice
        tensor({1, 0, 0}) = 7; tensor({1, 0, 1}) = 8; tensor({1, 0, 2}) = 9;
        tensor({1, 1, 0}) = 10; tensor({1, 1, 1}) = 11; tensor({1, 1, 2}) = 12;

        return tensor;
    }

    eccpp::mdarray<int> create3DTensorB() {
        eccpp::mdarray<int> tensor({3, 2, 2});
        // First 2x2 slice
        tensor({0, 0, 0}) = 1; tensor({0, 0, 1}) = 2;
        tensor({0, 1, 0}) = 3; tensor({0, 1, 1}) = 4;
        // Second 2x2 slice
        tensor({1, 0, 0}) = 5; tensor({1, 0, 1}) = 6;
        tensor({1, 1, 0}) = 7; tensor({1, 1, 1}) = 8;
        // Third 2x2 slice
        tensor({2, 0, 0}) = 9; tensor({2, 0, 1}) = 10;
        tensor({2, 1, 0}) = 11; tensor({2, 1, 1}) = 12;

        return tensor;
    }
};

TEST_F(MdarrayMulTest, MatrixMultiplication) {
    eccpp::mdarray<int> A = createMatrixA();
    eccpp::mdarray<int> B = createMatrixB();
    eccpp::mdarray<int> expected_AB = createExpectedAB();

    // Test * (opp == 0) and *= (opp == 1) operators for matrix multiplication
    for (int op = 0; op < 2; ++op) {
        eccpp::mdarray<int> AB = op == 0 ? A * B : A;
        if (op > 0)
            AB *= B;

        ASSERT_EQ(AB.dimensions().size(), 2);
        ASSERT_EQ(AB.dimensions()[0], 4);
        ASSERT_EQ(AB.dimensions()[1], 3);

        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                EXPECT_EQ(AB({i, j}), expected_AB({i, j}));
            }
        }

        // Check == operator too
        EXPECT_EQ(AB, expected_AB);
    }
}

TEST_F(MdarrayMulTest, MatrixMultiplicationIncompatible) {
    // Test matrices with incompatible dimensions
    eccpp::mdarray<int> A({2, 3});
    eccpp::mdarray<int> B({2, 2});

    // The * operator should throw for incompatible dimensions
    EXPECT_THROW(A * B, std::invalid_argument);
    EXPECT_THROW(A *= B, std::invalid_argument);
}

TEST_F(MdarrayMulTest, TensorMul) {
    // Test tensor Mul for higher dimensional tensors
    eccpp::mdarray<int> A = create3DTensor();  // 2x2x3
    eccpp::mdarray<int> B = create3DTensorB(); // 3x2x2

    eccpp::mdarray<int> C = A * B;

    // Expected dimensions of C: [2, 2, 2, 2] (removing A's dim 2 and B's dim 0)
    ASSERT_EQ(C.dimensions().size(), 4);
    ASSERT_EQ(C.dimensions()[0], 2); // from A
    ASSERT_EQ(C.dimensions()[1], 2); // from A
    ASSERT_EQ(C.dimensions()[2], 2); // from B
    ASSERT_EQ(C.dimensions()[3], 2); // from B

    // Test a few elements of the result
    // For example, C(0,0,0,0) = A(0,0,0)*B(0,0,0) + A(0,0,1)*B(1,0,0) + A(0,0,2)*B(2,0,0)
    // = 1*1 + 2*5 + 3*9 = 1 + 10 + 27 = 38
    EXPECT_EQ(C({0, 0, 0, 0}), 38);

    // C(1,1,1,1) = A(1,1,0)*B(0,1,1) + A(1,1,1)*B(1,1,1) + A(1,1,2)*B(2,1,1)
    // = 10*4 + 11*8 + 12*12 = 40 + 88 + 144 = 272
    EXPECT_EQ(C({1, 1, 1, 1}), 272);
}

TEST_F(MdarrayMulTest, DefaultMulOperator) {
    // Test the default behavior of * for higher dimensions
    eccpp::mdarray<int> A = create3DTensor();    // 2x2x3
    eccpp::mdarray<int> B = create3DTensorB();   // 3x2x2

    // * operator should contract the last dimension of A with the first dimension of B
    eccpp::mdarray<int> C = A * B;

    // Expected dimensions of C: [2, 2, 2, 2]
    ASSERT_EQ(C.dimensions().size(), 4);
    ASSERT_EQ(C.dimensions()[0], 2);
    ASSERT_EQ(C.dimensions()[1], 2);
    ASSERT_EQ(C.dimensions()[2], 2);
    ASSERT_EQ(C.dimensions()[3], 2);

    // Verify with some spot checks
    EXPECT_EQ(C({0, 0, 0, 0}), 38);
    EXPECT_EQ(C({1, 1, 1, 1}), 272);

    // Test *= operator
    eccpp::mdarray<int> A_copy = create3DTensor();
    A_copy *= B;

    // Verify dimensions
    ASSERT_EQ(A_copy.dimensions().size(), 4);

    // Verify values
    EXPECT_EQ(A_copy({0, 0, 0, 0}), 38);
    EXPECT_EQ(A_copy({1, 1, 1, 1}), 272);
}
