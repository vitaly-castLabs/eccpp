#include <gtest/gtest.h>

#include "mdarray.h"
#include "kron.h"

class KroneckerProductTest : public ::testing::Test {
protected:
    // Example matrices from Wikipedia: https://en.wikipedia.org/wiki/Kronecker_product#Examples

    // Example 1: A = [1 2; 3 4], B = [0 5; 6 7]
    eccpp::mdarray<int> createMatrixA1() {
        eccpp::mdarray<int> A({2, 2});
        A({0, 0}) = 1; A({0, 1}) = 2;
        A({1, 0}) = 3; A({1, 1}) = 4;
        return A;
    }

    eccpp::mdarray<int> createMatrixB1() {
        eccpp::mdarray<int> B({2, 2});
        B({0, 0}) = 0; B({0, 1}) = 5;
        B({1, 0}) = 6; B({1, 1}) = 7;
        return B;
    }

    eccpp::mdarray<int> createExpectedKron1() {
        // A ⊗ B = [1·B 2·B; 3·B 4·B] =
        // [1·0 1·5 2·0 2·5; 1·6 1·7 2·6 2·7; 3·0 3·5 4·0 4·5; 3·6 3·7 4·6 4·7] =
        // [0 5 0 10; 6 7 12 14; 0 15 0 20; 18 21 24 28]
        eccpp::mdarray<int> result({4, 4});
        result({0, 0}) = 0;  result({0, 1}) = 5;  result({0, 2}) = 0;  result({0, 3}) = 10;
        result({1, 0}) = 6;  result({1, 1}) = 7;  result({1, 2}) = 12; result({1, 3}) = 14;
        result({2, 0}) = 0;  result({2, 1}) = 15; result({2, 2}) = 0;  result({2, 3}) = 20;
        result({3, 0}) = 18; result({3, 1}) = 21; result({3, 2}) = 24; result({3, 3}) = 28;

        return result;
    }

    // Example 2: A = [1 -4 7; -2 3 3]
    eccpp::mdarray<int> createMatrixA2() {
        eccpp::mdarray<int> A({2, 3});
        A({0, 0}) = 1;  A({0, 1}) = -4; A({0, 2}) = 7;
        A({1, 0}) = -2; A({1, 1}) = 3;  A({1, 2}) = 3;
        return A;
    }

    // B = [8 -9 -6 5; 1 -3 -4 7; 2 8 -8 -3; 1 2 -5 -1]
    eccpp::mdarray<int> createMatrixB2() {
        eccpp::mdarray<int> B({4, 4});
        B({0, 0}) = 8;  B({0, 1}) = -9; B({0, 2}) = -6; B({0, 3}) = 5;
        B({1, 0}) = 1;  B({1, 1}) = -3; B({1, 2}) = -4; B({1, 3}) = 7;
        B({2, 0}) = 2;  B({2, 1}) = 8;  B({2, 2}) = -8; B({2, 3}) = -3;
        B({3, 0}) = 1;  B({3, 1}) = 2;  B({3, 2}) = -5; B({3, 3}) = -1;
        return B;
    }

    eccpp::mdarray<int> createExpectedKron2() {
        eccpp::mdarray<int> result({8, 12});

        result({0, 0}) = 8;   result({0, 1}) = -9;  result({0, 2}) = -6;  result({0, 3}) = 5;
        result({0, 4}) = -32; result({0, 5}) = 36;  result({0, 6}) = 24;  result({0, 7}) = -20;
        result({0, 8}) = 56;  result({0, 9}) = -63; result({0, 10}) = -42; result({0, 11}) = 35;

        result({1, 0}) = 1;   result({1, 1}) = -3;  result({1, 2}) = -4;  result({1, 3}) = 7;
        result({1, 4}) = -4;  result({1, 5}) = 12;  result({1, 6}) = 16;  result({1, 7}) = -28;
        result({1, 8}) = 7;   result({1, 9}) = -21; result({1, 10}) = -28; result({1, 11}) = 49;

        result({2, 0}) = 2;   result({2, 1}) = 8;   result({2, 2}) = -8;  result({2, 3}) = -3;
        result({2, 4}) = -8;  result({2, 5}) = -32; result({2, 6}) = 32;  result({2, 7}) = 12;
        result({2, 8}) = 14;  result({2, 9}) = 56;  result({2, 10}) = -56; result({2, 11}) = -21;

        result({3, 0}) = 1;   result({3, 1}) = 2;   result({3, 2}) = -5;  result({3, 3}) = -1;
        result({3, 4}) = -4;  result({3, 5}) = -8;  result({3, 6}) = 20;  result({3, 7}) = 4;
        result({3, 8}) = 7;   result({3, 9}) = 14;  result({3, 10}) = -35; result({3, 11}) = -7;

        result({4, 0}) = -16; result({4, 1}) = 18;  result({4, 2}) = 12;  result({4, 3}) = -10;
        result({4, 4}) = 24;  result({4, 5}) = -27; result({4, 6}) = -18; result({4, 7}) = 15;
        result({4, 8}) = 24;  result({4, 9}) = -27; result({4, 10}) = -18; result({4, 11}) = 15;

        result({5, 0}) = -2;  result({5, 1}) = 6;   result({5, 2}) = 8;   result({5, 3}) = -14;
        result({5, 4}) = 3;   result({5, 5}) = -9;  result({5, 6}) = -12; result({5, 7}) = 21;
        result({5, 8}) = 3;   result({5, 9}) = -9;  result({5, 10}) = -12; result({5, 11}) = 21;

        result({6, 0}) = -4;  result({6, 1}) = -16; result({6, 2}) = 16;  result({6, 3}) = 6;
        result({6, 4}) = 6;   result({6, 5}) = 24;  result({6, 6}) = -24; result({6, 7}) = -9;
        result({6, 8}) = 6;   result({6, 9}) = 24;  result({6, 10}) = -24; result({6, 11}) = -9;

        result({7, 0}) = -2;  result({7, 1}) = -4;  result({7, 2}) = 10;  result({7, 3}) = 2;
        result({7, 4}) = 3;   result({7, 5}) = 6;   result({7, 6}) = -15; result({7, 7}) = -3;
        result({7, 8}) = 3;   result({7, 9}) = 6;   result({7, 10}) = -15; result({7, 11}) = -3;

        return result;
    }

    // Example 3: non-square matrices
    eccpp::mdarray<int> createMatrixA3() {
        eccpp::mdarray<int> A({2, 3});  // 2×3 matrix
        A({0, 0}) = 1; A({0, 1}) = 2; A({0, 2}) = 3;
        A({1, 0}) = 4; A({1, 1}) = 5; A({1, 2}) = 6;
        return A;
    }

    eccpp::mdarray<int> createMatrixB3() {
        eccpp::mdarray<int> B({3, 2});  // 3×2 matrix
        B({0, 0}) = 7;  B({0, 1}) = 8;
        B({1, 0}) = 9;  B({1, 1}) = 10;
        B({2, 0}) = 11; B({2, 1}) = 12;
        return B;
    }

    eccpp::mdarray<int> createExpectedKron3() {
        // A ⊗ B is a (2×3) ⊗ (3×2) = (6×6) matrix
        eccpp::mdarray<int> result({6, 6});

        // Block (0,0): 1 * B
        result({0, 0}) = 7;  result({0, 1}) = 8;
        result({1, 0}) = 9;  result({1, 1}) = 10;
        result({2, 0}) = 11; result({2, 1}) = 12;

        // Block (0,1): 2 * B
        result({0, 2}) = 14; result({0, 3}) = 16;
        result({1, 2}) = 18; result({1, 3}) = 20;
        result({2, 2}) = 22; result({2, 3}) = 24;

        // Block (0,2): 3 * B
        result({0, 4}) = 21; result({0, 5}) = 24;
        result({1, 4}) = 27; result({1, 5}) = 30;
        result({2, 4}) = 33; result({2, 5}) = 36;

        // Block (1,0): 4 * B
        result({3, 0}) = 28; result({3, 1}) = 32;
        result({4, 0}) = 36; result({4, 1}) = 40;
        result({5, 0}) = 44; result({5, 1}) = 48;

        // Block (1,1): 5 * B
        result({3, 2}) = 35; result({3, 3}) = 40;
        result({4, 2}) = 45; result({4, 3}) = 50;
        result({5, 2}) = 55; result({5, 3}) = 60;

        // Block (1,2): 6 * B
        result({3, 4}) = 42; result({3, 5}) = 48;
        result({4, 4}) = 54; result({4, 5}) = 60;
        result({5, 4}) = 66; result({5, 5}) = 72;

        return result;
    }
};

TEST_F(KroneckerProductTest, KronProductNon2DThrows) {
    eccpp::mdarray<int> A3D({2, 2, 2});
    eccpp::mdarray<int> B2D({2, 2});
    eccpp::mdarray<int> C1D({5});

    EXPECT_THROW(eccpp::kron(A3D, B2D), std::invalid_argument);
    EXPECT_THROW(eccpp::kron(B2D, A3D), std::invalid_argument);
    EXPECT_THROW(eccpp::kron(A3D, C1D), std::invalid_argument);
    EXPECT_THROW(eccpp::kron(C1D, B2D), std::invalid_argument);
    EXPECT_THROW(eccpp::kron(B2D, C1D), std::invalid_argument);
}

TEST_F(KroneckerProductTest, Example1) {
    eccpp::mdarray<int> A = createMatrixA1();
    eccpp::mdarray<int> B = createMatrixB1();
    eccpp::mdarray<int> expected = createExpectedKron1();
    eccpp::mdarray<int> result = eccpp::kron(A, B);

    // Check dimensions
    ASSERT_EQ(result.dimensions().size(), 2);
    ASSERT_EQ(result.dimensions()[0], 4);
    ASSERT_EQ(result.dimensions()[1], 4);

    EXPECT_EQ(result, expected);
}

TEST_F(KroneckerProductTest, Example2) {
    eccpp::mdarray<int> A = createMatrixA2();
    eccpp::mdarray<int> B = createMatrixB2();
    eccpp::mdarray<int> expected = createExpectedKron2();
    eccpp::mdarray<int> result = eccpp::kron(A, B);

    EXPECT_EQ(result, expected);
}

TEST_F(KroneckerProductTest, Example3_NonSquareMatrices) {
    eccpp::mdarray<int> A = createMatrixA3();
    eccpp::mdarray<int> B = createMatrixB3();
    eccpp::mdarray<int> expected = createExpectedKron3();

    eccpp::mdarray<int> result = eccpp::kron(A, B);

    // Check dimensions
    ASSERT_EQ(result.dimensions().size(), 2);
    ASSERT_EQ(result.dimensions()[0], 6);
    ASSERT_EQ(result.dimensions()[1], 6);

    // Check equality with expected result
    EXPECT_EQ(result, expected);
}

TEST_F(KroneckerProductTest, CommutativityProperty) {
    // Kronecker product is not commutative: A ⊗ B ≠ B ⊗ A
    eccpp::mdarray<int> A = createMatrixA1();
    eccpp::mdarray<int> B = createMatrixB1();

    eccpp::mdarray<int> AB = eccpp::kron(A, B);
    eccpp::mdarray<int> BA = eccpp::kron(B, A);

    // Should be different
    EXPECT_NE(AB, BA);
}

TEST_F(KroneckerProductTest, ScalarMultiplicationProperty) {
    // Testing (cA) ⊗ B = A ⊗ (cB) = c(A ⊗ B)
    eccpp::mdarray<int> A = createMatrixA1();
    eccpp::mdarray<int> B = createMatrixB1();

    const int c = 3;

    eccpp::mdarray<int> cA = A * c;
    eccpp::mdarray<int> cB = B * c;

    eccpp::mdarray<int> cA_B = eccpp::kron(cA, B);
    eccpp::mdarray<int> A_cB = eccpp::kron(A, cB);
    eccpp::mdarray<int> c_AB = eccpp::kron(A, B) * c;

    // All should be equal
    EXPECT_EQ(cA_B, A_cB);
    EXPECT_EQ(cA_B, c_AB);
}

TEST_F(KroneckerProductTest, IncompatibleDimensions) {
    // Test with non-2D arrays
    eccpp::mdarray<int> A({2, 2});
    eccpp::mdarray<int> B({2, 2, 2});  // 3D array

    // The kron function should throw for non-2D matrices
    EXPECT_THROW(eccpp::kron(A, B), std::invalid_argument);
    EXPECT_THROW(eccpp::kron(B, A), std::invalid_argument);
}

TEST_F(KroneckerProductTest, FloatingPointMatrices) {
    // Test with floating point values
    eccpp::mdarray<double> A({2, 2});
    A({0, 0}) = 1.5; A({0, 1}) = 2.5;
    A({1, 0}) = 3.5; A({1, 1}) = 4.5;

    eccpp::mdarray<double> B({2, 2});
    B({0, 0}) = 0.5; B({0, 1}) = 1.5;
    B({1, 0}) = 2.5; B({1, 1}) = 3.5;

    eccpp::mdarray<double> result = eccpp::kron(A, B);

    // Expected: [0.75 2.25 1.25 3.75; 3.75 5.25 6.25 8.75; 1.75 5.25 2.25 6.75; 8.75 12.25 11.25 15.75]
    eccpp::mdarray<double> expected({4, 4});
    expected({0, 0}) = 0.75;  expected({0, 1}) = 2.25;  expected({0, 2}) = 1.25;  expected({0, 3}) = 3.75;
    expected({1, 0}) = 3.75;  expected({1, 1}) = 5.25;  expected({1, 2}) = 6.25;  expected({1, 3}) = 8.75;
    expected({2, 0}) = 1.75;  expected({2, 1}) = 5.25;  expected({2, 2}) = 2.25;  expected({2, 3}) = 6.75;
    expected({3, 0}) = 8.75;  expected({3, 1}) = 12.25; expected({3, 2}) = 11.25; expected({3, 3}) = 15.75;

    // Check each element with tolerance for floating point comparisons
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            EXPECT_NEAR(result({i, j}), expected({i, j}), 1e-10)
                << "Elements differ at (" << i << "," << j << ")";
        }
    }
}

TEST_F(KroneckerProductTest, IdentityMatrixProperty) {
    // Test with identity matrices: Im ⊗ In = Imn
    // Create 2x2 identity matrix
    eccpp::mdarray<int> I2({2, 2});
    I2({0, 0}) = 1; I2({0, 1}) = 0;
    I2({1, 0}) = 0; I2({1, 1}) = 1;

    // Kronecker product of I2 with itself should be a 4x4 identity matrix
    eccpp::mdarray<int> result = eccpp::kron(I2, I2);

    // Expected 4x4 identity matrix
    eccpp::mdarray<int> I4({4, 4});
    I4({0, 0}) = 1; I4({0, 1}) = 0; I4({0, 2}) = 0; I4({0, 3}) = 0;
    I4({1, 0}) = 0; I4({1, 1}) = 1; I4({1, 2}) = 0; I4({1, 3}) = 0;
    I4({2, 0}) = 0; I4({2, 1}) = 0; I4({2, 2}) = 1; I4({2, 3}) = 0;
    I4({3, 0}) = 0; I4({3, 1}) = 0; I4({3, 2}) = 0; I4({3, 3}) = 1;

    EXPECT_EQ(result, I4);
}
