// various code puncturing experiments

#include <iostream>
#include <array>
#include <iomanip>
#include <chrono>

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

    std::cout << "----------------------------------------\n";
}

int lowRatePuncturing(const size_t N, const size_t target_N, const std::vector<size_t>& info_bits, const float noise_level, int iterations) {
    const std::uint_fast32_t permutation_seed = 1234567;
    eccpp::polar_enc_butterfly enc(N, permutation_seed);
    eccpp::polar_dec<float> dec(N, permutation_seed);
    std::vector<int> msg_with_frozen_bits(N);
    std::vector<int> msg(info_bits.size());

    // deterministic LCG for reproducible results
    std::minstd_rand rg;
    rg.seed(54321);

    std::vector<float> llr_noise(N);

    int success = 0;
    for (int i = 0; i < iterations; ++i) {
        // generate random message and noise
        std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });
        std::generate(llr_noise.begin(), llr_noise.end(), [&rg, noise_level]() {
                // footgun: rg() returns unsigned int, we need to convert it to
                // signed to do arithmetics one line below
                const int noise = rg() % 5;
                return (noise - 2) * noise_level * 0.5f;
            });

        // combine with frozen bits
        for (size_t i = 0; i < info_bits.size(); ++i)
            msg_with_frozen_bits[info_bits[i]] = msg[i];
        // encode
        auto codeword = enc.encode(msg_with_frozen_bits);

        auto llr = bits_to_llr(codeword);
        for (size_t i = 0; i < N; ++i)
            llr[i] += llr_noise[i];

        for (int i = target_N; i < N; ++i)
            llr[i] = 0;

        auto result = dec.decode(llr, info_bits);
        if (result.msg == msg)
            success++;
    }

    return success;
}

/*
    Results for iterations = 100 (i.e. larger transform performs worse):
    SNR: -23.5 dB
    N =  8192: 100.0% success
    N = 32768: 100.0% success

    SNR: -26.0 dB
    N =  8192: 96.0% success
    N = 32768: 97.0% success

    SNR: -28.0 dB
    N =  8192: 81.0% success
    N = 32768: 78.0% success

    SNR: -29.5 dB
    N =  8192: 45.0% success
    N = 32768: 44.0% success

    SNR: -30.9 dB
    N =  8192: 27.0% success
    N = 32768: 18.0% success
*/
void lowRatePuncturingWithDifferentN(int iterations) {
    std::cout << "\n16 info bits in 8000-bit message:\n";

    const size_t N1 = 8192;
    const size_t N2 = 32768;
    const size_t target_N = 8000;
    // 14-bit set with Hamming distance 4096 + 2 bits from the Hamming distance 2048 set
    const std::vector<size_t> info_bits1 = {2047, 3071, 4095, 6143, 7167, 7679, 7935, 8063, 8127, 8159, 8175, 8183, 8187, 8189, 8190, 8191};
    // "natural" 16-bit set with Hamming distance 16384
    const std::vector<size_t> info_bits2 = {16383, 24575, 28671, 30719, 31743, 32255, 32511, 32639, 32703, 32735, 32751, 32759, 32763, 32765, 32766, 32767};

    const std::vector<float> noise_levels = {150, 200, 250, 300, 350};

    for (auto noise_level: noise_levels) {
        const auto n1_start = std::chrono::steady_clock::now();
        const auto n1_success = lowRatePuncturing(N1, target_N, info_bits1, noise_level, iterations);
        const auto n2_start = std::chrono::steady_clock::now();
        const auto n2_success = lowRatePuncturing(N2, target_N, info_bits2, noise_level, iterations);
        const auto n2_end = std::chrono::steady_clock::now();

        const auto n1_sec = std::chrono::duration_cast<std::chrono::seconds>(n2_start - n1_start).count();
        const auto n2_sec = std::chrono::duration_cast<std::chrono::seconds>(n2_end - n2_start).count();
        const auto snr_db = 20.0f * std::log10(10.0 / noise_level);
        std::cout << "\nSNR: " << std::fixed << std::setprecision(1) << snr_db << " dB\n";
        std::cout << "  N = " << std::setw(5) << N1 << ": " << std::fixed << std::setprecision(1) << 100.0 * n1_success / iterations << "% success, time: " << n1_sec << " s\n";
        std::cout << "  N = " << std::setw(5) << N2 << ": " << std::fixed << std::setprecision(1) << 100.0 * n2_success / iterations << "% success, time: " << n2_sec << " s\n";
    }
    std::cout << "----------------------------------------\n";
}

int main() {
    halfRatePuncturing(250);
    lowRatePuncturingWithDifferentN(4);

    std::cout << "\nDone\n";
}
