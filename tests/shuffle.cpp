#include <gtest/gtest.h>

#include "shuffle.h"

TEST(ShuffleTest, ShuffleUnshuffle) {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> expected = vec;

    for (auto seed = 0; seed < 100; ++seed) {
        eccpp::shuffle(vec, seed);
        EXPECT_NE(vec, expected);
        eccpp::unshuffle(vec, seed);
        EXPECT_EQ(vec, expected);
    }
}
