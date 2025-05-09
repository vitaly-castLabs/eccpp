#include <vector>
#include <algorithm>
#include <memory>

namespace eccpp {

//
// longest common run (= contiguous subsequence), not to be confused with LC subsequence below.
// The difference is that the term subsequence allows dropping elements (from both a and b):
// lc_run({1, 0, 1, 0, 1}, {1, 1, 1}) is 1 while lc_subseq(..) is 3 (after zeroes are dropped)
//
template <typename T, typename SizeType = int>
size_t lc_run(const std::vector<T>& a, const std::vector<T>& b) {
    auto& shorter = (a.size() < b.size()) ? a : b;
    auto& longer = (std::addressof(shorter) == std::addressof(a)) ? b : a;

    auto n = shorter.size();
    auto m = longer.size();
    SizeType max_len = 0;

    std::vector<SizeType> prev(n + 1);
    std::vector<SizeType> curr(n + 1);

    for (auto i = 1; i <= m; ++i) {
        for (auto j = 1; j <= n; ++j) {
            if (longer[i - 1] == shorter[j - 1]) {
                curr[j] = prev[j - 1] + 1;
                max_len = std::max(max_len, curr[j]);
            }
            else
                curr[j] = 0;
        }
        std::swap(prev, curr);
    }

    return max_len;
}

template <typename T, typename SizeType = int>
size_t lc_subseq(const std::vector<T>& a, const std::vector<T>& b) {
    auto& shorter = (a.size() < b.size()) ? a : b;
    auto& longer = (std::addressof(shorter) == std::addressof(a)) ? b : a;

    auto n = shorter.size();
    auto m = longer.size();

    std::vector<SizeType> prev(n + 1);
    std::vector<SizeType> curr(n + 1);

    for (auto i = 1; i <= m; ++i) {
        for (auto j = 1; j <= n; ++j) {
            if (longer[i - 1] == shorter[j - 1])
                curr[j] = prev[j - 1] + 1;
            else
                curr[j] = std::max(prev[j], curr[j - 1]);
        }
        std::swap(prev, curr);
    }

    return prev[n];
}

//
// longest common run, but with the same starting index for both vectors, tbh I'd call it longest overlap:
// a = {1, 1, 1, 0, 0}
// b = {0, 0, 1, 1, 1}
//            ^ - this is the only overlapping symbol
// lc_aligned_run(a, b) is 1, while lc_run(a, b) is 3
//
template <typename T, typename SizeType = int>
SizeType lc_aligned_run(const std::vector<T>& a, const std::vector<T>& b) {
    SizeType max_len = 0;
    SizeType curr_len = 0;
    SizeType len = std::min<SizeType>(a.size(), b.size());

    for (SizeType i = 0; i < len; ++i) {
        if (a[i] == b[i]) {
            ++curr_len;
            max_len = std::max(max_len, curr_len);
        }
        else
            curr_len = 0;
    }

    return max_len;
}

} // namespace eccpp