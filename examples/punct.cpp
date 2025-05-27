// various code puncturing experiments

#include <iostream>
#include <array>
#include <iomanip>

#include "polar-enc.h"
#include "polar-dec.h"

#include "./shared.h"

void halfRatePuncturing(int iterations) {
    std::cout << "\nHalf-rate patterns: drop first half (0), last half (1), odd bits (2), even bits (3).\n" <<
        "Decoding success rates under very strong noise:\n";

    const size_t N = 1024;
    enum pattern {
        drop_first_half = 0,
        drop_last_half,
        drop_odd,
        drop_even,
        num_patterns
    };
    std::array<int, num_patterns> results = {0};

    const std::uint_fast32_t permutation_seed = 301;
    eccpp::polar_enc_butterfly enc(N, permutation_seed);
    eccpp::polar_dec<float> dec(N, permutation_seed);
    std::vector<int> msg_with_frozen_bits(N);
    std::vector<size_t> info_bits = {511, 767, 895, 959, 991, 1007, 1015, 1019, 1021, 1022, 1023};

    std::vector<int> msg(info_bits.size());

    // deterministic LCG for reproducible results
    std::minstd_rand rg;
    rg.seed(54321);

    std::vector<float> llr_noise(N);

    for (int i = 0; i < iterations; ++i) {
        // generate random message and noise
        std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });
        std::generate(llr_noise.begin(), llr_noise.end(), [&rg]() {
            // footgun: rg() returns unsigned int, we need to convert it to
            // signed to do arithmetics one line below
            const int noise = rg() % 5;
            return (noise - 2) * 25; // -50, -25, 0, +25 or +50
        });

        // combine with frozen bits
        for (size_t i = 0; i < info_bits.size(); ++i)
            msg_with_frozen_bits[info_bits[i]] = msg[i];
        // encode
        auto codeword = enc.encode(msg_with_frozen_bits);

        for (auto pattern = 0; pattern < num_patterns; ++pattern) {
            auto cw = codeword;
            switch (pattern) {
                case drop_first_half:
                    for (size_t i = 0; i < N / 2; ++i)
                        cw[i] = -1;
                    break;
                case drop_last_half:
                    for (size_t i = N / 2; i < N; ++i)
                        cw[i] = -1;
                    break;
                case drop_odd:
                    for (size_t i = 0; i < N; i += 2)
                        cw[i] = -1;
                    break;
                case drop_even:
                    for (size_t i = 1; i < N; i += 2)
                        cw[i] = -1;
                    break;
            }
            auto llr = bits_to_llr(cw);
            for (size_t i = 0; i < N; ++i)
                llr[i] += llr_noise[i];

            auto result = dec.decode(llr, info_bits);
            if (result.msg == msg)
                results[pattern]++;
        }
    }

    for (auto pattern = 0; pattern < num_patterns; ++pattern)
        std::cout << "  " << pattern << ": " << std::fixed << std::setprecision(1) << 100.0 * results[pattern] / iterations << "% success\n";
}

int main() {
    halfRatePuncturing(100);
}