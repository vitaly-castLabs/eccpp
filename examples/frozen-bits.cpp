//
// figure out which bits are reliable / "polarized the most" vs insignificant ones, which
// should be designated as frozen bits. If you look at the shape of the generator matrix
// G_n, the first bit is only utilized once (= prime frozen bit candidate), while the last
// one contributes to all the encoded bits since the last row of G_n is all 1s (= the most
// reliable bit).
//

#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

#include "gn.h"

constexpr size_t N_START = 8;
constexpr size_t N_END = 16384;

struct row {
    size_t idx = 0;
    int one_cnt = 0;
};

int main() {
    size_t start_rows_to_print = N_START / 2;
    for (size_t N = N_START; N <= N_END; N *= 2) {
        std::cout << "\n\nN = " << N << "\n";
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

        std::cout << "---------------------------------\n" <<
                     " row  |    1s | rows with more 1s\n" <<
                     "---------------------------------\n";

        const size_t end_rows_to_print = start_rows_to_print / 4;
        size_t idx = 0;
        bool printed_dots = false;
        auto prev_cnt = rows[0].one_cnt;
        std::vector<std::vector<size_t>> info_bit_sets(1);
        const size_t num_bit_sets = 3;
        for (const auto& r: rows) {
            if (idx < start_rows_to_print || idx >= N - end_rows_to_print)
                std::cout << std::setw(5) << r.idx << " | " << std::setw(5) << r.one_cnt << " | " << (r.one_cnt != prev_cnt ? std::to_string(idx) : "") << "\n";
            else if (!printed_dots)
                std::cout << "...\n", printed_dots = true;

            if (info_bit_sets.size() <= num_bit_sets) {
                if (r.one_cnt != prev_cnt) {
                    info_bit_sets.emplace_back();
                    // previous (more "polarized" / larger distance) sets are included into subsequent ones
                    if (info_bit_sets.size() <= num_bit_sets)
                        info_bit_sets.back() = info_bit_sets.end()[-2];
                }
                if (info_bit_sets.size() <= num_bit_sets)
                    info_bit_sets.back().push_back(r.idx);
            }
            prev_cnt = r.one_cnt;
            ++idx;
        }

        std::cout << "\nInfo bit indices:\n";
        auto NN = N;
        for (auto& info_bits: info_bit_sets) {
            if (info_bits.empty())
                break;

            std::sort(info_bits.begin(), info_bits.end());

            std::cout << "Dist " << std::setw(5) << NN << " [" << std::setw(3) << info_bits.size() << "]: {";
            for (size_t i = 0; i < info_bits.size(); ++i) {
                std::cout << info_bits[i];
                if (i < info_bits.size() - 1)
                    std::cout << ", ";
            }
            std::cout << "}\n";
            NN /= 2;
        }

        start_rows_to_print += 4;
    }
}
