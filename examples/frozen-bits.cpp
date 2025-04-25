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

#include "gn.h"

struct row {
    size_t idx = 0;
    int one_cnt = 0;
};

int main() {
    const size_t N = 8192;
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

    return 0;
}
