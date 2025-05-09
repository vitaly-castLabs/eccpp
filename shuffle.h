//
// deterministic pseudorandom reversible in-place shuffling for elements of std:: containers
// supporting random access iterators
//

#ifndef ECCPP_SHUFFLE_H
#define ECCPP_SHUFFLE_H

#include <vector>
#include <random>
#include <algorithm>
#include <cstdint>

namespace eccpp {

template<typename T>
void shuffle(T& container, std::uint_fast32_t seed) {
    if (container.size() < 2)
        return;

    std::minstd_rand rng(seed);
    for (size_t i = container.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        size_t j = dist(rng);
        std::swap(container[i], container[j]);
    }
}

template<typename T>
void unshuffle(T& container, std::uint_fast32_t seed) {
    if (container.size() < 2)
        return;

    std::minstd_rand rng(seed);
    std::vector<size_t> indices(container.size());
    for (size_t i = 0; i < container.size(); ++i)
        indices[i] = i;

    for (size_t i = indices.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        size_t j = dist(rng);
        std::swap(indices[i], indices[j]);
    }

    T temp = container;
    for (size_t i = 0; i < container.size(); ++i)
        container[indices[i]] = temp[i];
}

} // namespace eccpp

#endif // ECCPP_SHUFFLE_H
