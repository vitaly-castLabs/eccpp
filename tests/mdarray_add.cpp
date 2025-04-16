#include <gtest/gtest.h>

#include "mdarray.h"

class MdarrayAddTest : public ::testing::Test {
protected:
    // Create a 2x3 matrix filled with consecutive integers
    eccpp::mdarray<int> createMatrix2x3() {
        eccpp::mdarray<int> A({2, 3});
        A({0, 0}) = 1; A({0, 1}) = 2; A({0, 2}) = 3;
        A({1, 0}) = 4; A({1, 1}) = 5; A({1, 2}) = 6;
        return A;
    }

    // Create another 2x3 matrix with different values
    eccpp::mdarray<int> createMatrix2x3B() {
        eccpp::mdarray<int> B({2, 3});
        B({0, 0}) = 2; B({0, 1}) = 3; B({0, 2}) = 4;
        B({1, 0}) = 5; B({1, 1}) = 6; B({1, 2}) = 7;
        return B;
    }

    // Create a 3x2 matrix (different dimensions than above)
    eccpp::mdarray<int> createMatrix3x2() {
        eccpp::mdarray<int> C({3, 2});
        C({0, 0}) = 1; C({0, 1}) = 2;
        C({1, 0}) = 3; C({1, 1}) = 4;
        C({2, 0}) = 5; C({2, 1}) = 6;
        return C;
    }

    // Create a 3D tensor
    eccpp::mdarray<int> create3DTensor() {
        eccpp::mdarray<int> T({2, 2, 2});
        T({0, 0, 0}) = 1; T({0, 0, 1}) = 2;
        T({0, 1, 0}) = 3; T({0, 1, 1}) = 4;
        T({1, 0, 0}) = 5; T({1, 0, 1}) = 6;
        T({1, 1, 0}) = 7; T({1, 1, 1}) = 8;
        return T;
    }

    // Create another 3D tensor with different values
    eccpp::mdarray<int> create3DTensorB() {
        eccpp::mdarray<int> T({2, 2, 2});
        T({0, 0, 0}) = 8; T({0, 0, 1}) = 7;
        T({0, 1, 0}) = 6; T({0, 1, 1}) = 5;
        T({1, 0, 0}) = 4; T({1, 0, 1}) = 3;
        T({1, 1, 0}) = 2; T({1, 1, 1}) = 1;
        return T;
    }
};

TEST_F(MdarrayAddTest, Addition) {
    eccpp::mdarray<int> A = createMatrix2x3();
    eccpp::mdarray<int> B = createMatrix2x3B();

    // Test addition operator
    eccpp::mdarray<int> sum = A + B;

    // Verify dimensions
    ASSERT_EQ(sum.dimensions().size(), 2);
    ASSERT_EQ(sum.dimensions()[0], 2);
    ASSERT_EQ(sum.dimensions()[1], 3);

    // Verify values (should be element-wise sum)
    EXPECT_EQ(sum({0, 0}), 3);  // 1 + 2
    EXPECT_EQ(sum({0, 1}), 5);  // 2 + 3
    EXPECT_EQ(sum({0, 2}), 7);  // 3 + 4
    EXPECT_EQ(sum({1, 0}), 9);  // 4 + 5
    EXPECT_EQ(sum({1, 1}), 11); // 5 + 6
    EXPECT_EQ(sum({1, 2}), 13); // 6 + 7
}

TEST_F(MdarrayAddTest, AdditionAssignment) {
    eccpp::mdarray<int> A = createMatrix2x3();
    eccpp::mdarray<int> B = createMatrix2x3B();

    // Store original values of A for comparison
    int a00 = A({0, 0}), a01 = A({0, 1}), a02 = A({0, 2});
    int a10 = A({1, 0}), a11 = A({1, 1}), a12 = A({1, 2});

    // Test addition assignment operator
    A += B;

    // Verify dimensions (should be unchanged)
    ASSERT_EQ(A.dimensions().size(), 2);
    ASSERT_EQ(A.dimensions()[0], 2);
    ASSERT_EQ(A.dimensions()[1], 3);

    // Verify values (should be original A + B)
    EXPECT_EQ(A({0, 0}), a00 + B({0, 0}));
    EXPECT_EQ(A({0, 1}), a01 + B({0, 1}));
    EXPECT_EQ(A({0, 2}), a02 + B({0, 2}));
    EXPECT_EQ(A({1, 0}), a10 + B({1, 0}));
    EXPECT_EQ(A({1, 1}), a11 + B({1, 1}));
    EXPECT_EQ(A({1, 2}), a12 + B({1, 2}));
}

TEST_F(MdarrayAddTest, Subtraction) {
    eccpp::mdarray<int> A = createMatrix2x3();
    eccpp::mdarray<int> B = createMatrix2x3B();

    // Test subtraction operator
    eccpp::mdarray<int> diff = A - B;

    // Verify dimensions
    ASSERT_EQ(diff.dimensions().size(), 2);
    ASSERT_EQ(diff.dimensions()[0], 2);
    ASSERT_EQ(diff.dimensions()[1], 3);

    // Verify values (should be element-wise difference)
    EXPECT_EQ(diff({0, 0}), -1); // 1 - 2
    EXPECT_EQ(diff({0, 1}), -1); // 2 - 3
    EXPECT_EQ(diff({0, 2}), -1); // 3 - 4
    EXPECT_EQ(diff({1, 0}), -1); // 4 - 5
    EXPECT_EQ(diff({1, 1}), -1); // 5 - 6
    EXPECT_EQ(diff({1, 2}), -1); // 6 - 7
}

TEST_F(MdarrayAddTest, SubtractionAssignment) {
    eccpp::mdarray<int> A = createMatrix2x3();
    eccpp::mdarray<int> B = createMatrix2x3B();

    // Store original values of A for comparison
    int a00 = A({0, 0}), a01 = A({0, 1}), a02 = A({0, 2});
    int a10 = A({1, 0}), a11 = A({1, 1}), a12 = A({1, 2});

    // Test subtraction assignment operator
    A -= B;

    // Verify dimensions (should be unchanged)
    ASSERT_EQ(A.dimensions().size(), 2);
    ASSERT_EQ(A.dimensions()[0], 2);
    ASSERT_EQ(A.dimensions()[1], 3);

    // Verify values (should be original A - B)
    EXPECT_EQ(A({0, 0}), a00 - B({0, 0}));
    EXPECT_EQ(A({0, 1}), a01 - B({0, 1}));
    EXPECT_EQ(A({0, 2}), a02 - B({0, 2}));
    EXPECT_EQ(A({1, 0}), a10 - B({1, 0}));
    EXPECT_EQ(A({1, 1}), a11 - B({1, 1}));
    EXPECT_EQ(A({1, 2}), a12 - B({1, 2}));
}

TEST_F(MdarrayAddTest, Negation) {
    eccpp::mdarray<int> A = createMatrix2x3();

    // Test unary minus operator
    eccpp::mdarray<int> neg = -A;

    // Verify dimensions
    ASSERT_EQ(neg.dimensions().size(), 2);
    ASSERT_EQ(neg.dimensions()[0], 2);
    ASSERT_EQ(neg.dimensions()[1], 3);

    // Verify values (should be negation of original values)
    EXPECT_EQ(neg({0, 0}), -1);
    EXPECT_EQ(neg({0, 1}), -2);
    EXPECT_EQ(neg({0, 2}), -3);
    EXPECT_EQ(neg({1, 0}), -4);
    EXPECT_EQ(neg({1, 1}), -5);
    EXPECT_EQ(neg({1, 2}), -6);
}

TEST_F(MdarrayAddTest, IncompatibleDimensions) {
    eccpp::mdarray<int> A = createMatrix2x3();    // 2x3
    eccpp::mdarray<int> C = createMatrix3x2();    // 3x2

    // Operations between arrays with different dimensions should throw
    EXPECT_THROW(A + C, std::invalid_argument);
    EXPECT_THROW(A - C, std::invalid_argument);
    EXPECT_THROW(A += C, std::invalid_argument);
    EXPECT_THROW(A -= C, std::invalid_argument);
}

TEST_F(MdarrayAddTest, HigherDimensionalArrays) {
    eccpp::mdarray<int> T1 = create3DTensor();
    eccpp::mdarray<int> T2 = create3DTensorB();

    // Test addition of 3D tensors
    eccpp::mdarray<int> sum = T1 + T2;

    // Verify dimensions
    ASSERT_EQ(sum.dimensions().size(), 3);
    ASSERT_EQ(sum.dimensions()[0], 2);
    ASSERT_EQ(sum.dimensions()[1], 2);
    ASSERT_EQ(sum.dimensions()[2], 2);

    // Verify some values
    EXPECT_EQ(sum({0, 0, 0}), 9);  // 1 + 8
    EXPECT_EQ(sum({0, 0, 1}), 9);  // 2 + 7
    EXPECT_EQ(sum({1, 1, 0}), 9);  // 7 + 2
    EXPECT_EQ(sum({1, 1, 1}), 9);  // 8 + 1

    // Test subtraction of 3D tensors
    eccpp::mdarray<int> diff = T1 - T2;

    // Verify some values
    EXPECT_EQ(diff({0, 0, 0}), -7); // 1 - 8
    EXPECT_EQ(diff({0, 0, 1}), -5); // 2 - 7
    EXPECT_EQ(diff({1, 1, 0}), 5);  // 7 - 2
    EXPECT_EQ(diff({1, 1, 1}), 7);  // 8 - 1

    // Test in-place operations
    eccpp::mdarray<int> T1_copy = create3DTensor();
    T1_copy += T2;

    // Verify some values
    EXPECT_EQ(T1_copy({0, 0, 0}), 9);
    EXPECT_EQ(T1_copy({1, 1, 1}), 9);

    eccpp::mdarray<int> T1_copy2 = create3DTensor();
    T1_copy2 -= T2;

    // Verify some values
    EXPECT_EQ(T1_copy2({0, 0, 0}), -7);
    EXPECT_EQ(T1_copy2({1, 1, 1}), 7);
}

TEST_F(MdarrayAddTest, ChainedAdd) {
    eccpp::mdarray<int> A = createMatrix2x3();
    eccpp::mdarray<int> B = createMatrix2x3B();

    // Test chained operations
    eccpp::mdarray<int> result = A + B - A;
    EXPECT_EQ(result, B);

    // Test chained operations with negation
    eccpp::mdarray<int> result2 = -A + B;

    // Expected values
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(result2({i, j}), B({i, j}) - A({i, j}));
        }
    }
}
