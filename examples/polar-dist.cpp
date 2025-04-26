// Generate codewords for all possible information bit combinations
// and calculate the minimum Hamming distance

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>

#include "gn.h"
#include "polar-enc.h"
#include "hamdist.h"

//#define N 8192
#define N 2048
//#define N 256

std::string chronoToHms(std::chrono::duration<int, std::milli> duration) {
    const int total_ms = duration.count();
    const int h = total_ms / 3600000;
    const int m = (total_ms % 3600000) / 60000;
    const int s = (total_ms % 60000) / 1000;
    const int ms = total_ms % 1000;

    std::ostringstream oss;
    if (h > 0)
        oss << std::setfill('0') << h << "h";
    if (h > 0 || m > 0)
        oss << std::setw(oss.tellp() ? 0 : 2) << std::setfill('0') << m << "m";
    oss << std::setw(oss.tellp() ? 0 : 2) << std::setfill('0') << s << "."
        << std::setw(3) << std::setfill('0') << ms << "s";

    return oss.str();
}

int main() {
    eccpp::polar_enc_butterfly enc(N);
    size_t min_dist = N + 1;

    // these indices are produced with frozen-bits.cpp
    const std::vector<size_t> info_bits =
#if N == 8192
        // top rows with 8192 and 4096 ones
        {8191, 8190, 8189, 8187, 8183, 8175, 8159, 8127, 8063, 7935, 7679, 7167, 6143, 4095};
#elif N == 2048
        // top rows with 2048 and 1024 ones
        {2047, 2046, 2045, 2043, 2039, 2031, 2015, 1983, 1919, 1791, 1535, 1023};
#elif N == 256
        // top rows with 256 and 128 ones
        {255, 254, 253, 251, 247, 239, 223, 191, 127};
#endif

    eccpp::mdarray<int> msg({N});
    for (size_t i = 0; i < N; ++i)
        msg({i}) = 0;

    const size_t codeword_count = 1 << info_bits.size();
    std::vector<eccpp::mdarray<int>> codewords;
    codewords.reserve(codeword_count);

    std::cout << "\nGenerating codewords, might take a while...\n\n";
    const auto start = std::chrono::steady_clock::now();
    while (codewords.size() < codeword_count) {
        const auto codeword = enc.encode(msg);
        for (const auto& cw: codewords) {
            const auto dist = eccpp::hamdist(cw, codeword);
            if (dist == 0)
                throw std::runtime_error("Hamming distance is 0 - identical codewords");

            min_dist = std::min(min_dist, dist);
        }
        codewords.push_back(codeword);

        // increment message
        for (size_t i = 0; i < info_bits.size(); ++i) {
            auto& bit = msg({info_bits[i]});
            bit ^= 1;

            if (bit) {
                for (size_t j = 0; j < i; ++j)
                    msg({info_bits[j]}) = 0;

                break;
            }
        }

        std::cout << "\rProcessed " << codewords.size() << " of " << codeword_count << " codewords (min dist: " << min_dist << ")     ";
    }
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\n\nDone in " << chronoToHms(elapsed) << "\n";

    std::cout << "\nMinimum Hamming distance:     " << min_dist << "\n";
    std::cout << "Minimum repeat code distance: " << N / info_bits.size() << "\n";
}
