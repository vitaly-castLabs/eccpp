//
// figure out which bits are reliable / "polarized the most" vs insignificant ones, which
// should be designated as frozen bits. If you look at the shape of the generator matrix
// G_n, the first bit is only utilized once (= prime frozen bit candidate), while the last
// one contributes to all the encoded bits since the last row of G_n is all 1s (= the most
// reliable bit).
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <chrono>

#include "gn.h"
#include "polar-enc.h"
#include "hamdist.h"

//#define N 8192
#define N 2048
//#define N 256

struct row {
    size_t idx = 0;
    int one_cnt = 0;
};

int main() {
    const auto gn = eccpp::gn(N);

    std::vector<row> rows;
    rows.reserve(N);
    for (size_t r = 0; r < N; ++r) {
        int num_ones = 0;
        for (size_t c = 0; c < N; ++c)
            num_ones += gn({r, c});

        rows.push_back({r, num_ones});
    }

    std::sort(rows.begin(), rows.end(), [](const row& a, const row& b) {
        if (a.one_cnt == b.one_cnt)
            return a.idx > b.idx;
        return a.one_cnt > b.one_cnt;
    });

    std::cout << " row -   1s - rows with more 1s\n" <<
                 "-------------------------------\n";

    const size_t start_rows_to_print = 100;
    const size_t end_rows_to_print = 20;
    size_t idx = 0;
    bool printed_dots = false;
    auto prev_cnt = rows[0].one_cnt;
    for (const auto& r: rows) {
        if (idx < start_rows_to_print || idx >= N - end_rows_to_print)
            std::cout << std::setw(4) << r.idx << " - " << std::setw(4) << r.one_cnt << (r.one_cnt != prev_cnt ? " - " + std::to_string(idx) : "") << "\n";
        else if (!printed_dots)
            std::cout << "...\n(frozen bits candidates below)\n...\n", printed_dots = true;

        prev_cnt = r.one_cnt;
        ++idx;
    }

    // stage 2: generate codewords for all possible information bit combinations
    // and calculate the minimum Hamming distance to compare it with repeat code
    eccpp::polar_enc_butterfly enc(N);
    size_t min_dist = N + 1;

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

        std::cout << "\rProcessed " << codewords.size() << " of " << codeword_count << " codewords (min dist: " << min_dist << ")";
    }
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "\n\nDone in " << elapsed.count() << " seconds\n";

    std::cout << "\nMinimum Hamming distance:  " << min_dist << "\n";
    std::cout << "Minimum repeat code distance: " << N / info_bits.size() << "\n";

    return 0;
}
