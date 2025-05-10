#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "polar-enc.h"
#include "polar-dec.h"
#include "shuffle.h"

constexpr size_t N = 8192;

std::vector<float> bits_to_llr(const std::vector<int>& bits) {
    std::vector<float> llr(bits.size());
    for (size_t i = 0; i < bits.size(); ++i) {
        const auto bit = bits[i];
        // negative (-1 usually) value is utilized to represent erasure / unknown bit
        llr[i] = bit < 0 ? 0 : (bit ? -10 : 10);
    }
    return llr;
}

int main(int argc, char **argv) {
    eccpp::polar_enc_butterfly enc(N);
    eccpp::polar_dec<float> dec(N);
    std::vector<int> msg_with_frozen_bits(N);
    std::vector<size_t> info_bits = {4095, 6143, 7167, 7679, 7935, 8063, 8127, 8159, 8175, 8183, 8187, 8189, 8190, 8191};

    std::vector<int> msg(info_bits.size());

    // deterministic LCG for reproducible results
    std::minstd_rand rg;
    rg.seed(987654321);

    const std::uint_fast32_t shuffle_seed = 12345;

    // crop test
    std::cout << "Crop test. Wait for it (every iteration takes a couple of minutes)...\n\n";

    // A test run for future reference:
    // N = 8192, rg's seed = 987654321, shuffle seed = 12345, num_iter = 500
    // info_bits[14] = {4095, 6143, 7167, 7679, 7935, 8063, 8127, 8159, 8175, 8183, 8187, 8189, 8190, 8191}
    // ------------------------------------------------------------------------------------------------------
    // Crop size: 15, success: 77.6%, fail: 22.4%
    // Crop size: 16, success: 87.8%, fail: 12.2%
    // Crop size: 17, success: 93.6%, fail: 6.4%
    // Crop size: 18, success: 98.6%, fail: 1.4%
    // Crop size: 19, success: 99.2%, fail: 0.8%
    // Crop size: 20, success: 99.0%, fail: 1.0%
    // Crop size: 21, success: 99.8%, fail: 0.2%
    // Crop size: 22, success: 100.0%, fail: 0.0%
    // Crop size: 23, success: 100.0%, fail: 0.0%
    // Crop size: 24, success: 100.0%, fail: 0.0%
    // Crop size: 25, success: 100.0%, fail: 0.0%
    // Crop size: 26, success: 100.0%, fail: 0.0%
    // Crop size: 27, success: 99.8%, fail: 0.2%
    // Crop size: 28, success: 100.0%, fail: 0.0%
    // Crop size: 29, success: 100.0%, fail: 0.0%
    // Crop size: 30, success: 100.0%, fail: 0.0%
    //
    // Average codeword decode time: 162 ms
    // ------------------------------------------------------------------------------------------------------
    // And this is for the 5G NR Matlab code, slightly different params though: 13 / 8040 bits:
    // Crop: 15 bits, decoding success: 10.9%, wrong corrections: 11.8%
    // Crop: 16 bits, decoding success: 15.7%, wrong corrections: 10.6%
    // Crop: 17 bits, decoding success: 28.7%, wrong corrections: 8.1%
    // Crop: 18 bits, decoding success: 44.7%, wrong corrections: 6.1%
    // Crop: 19 bits, decoding success: 60.9%, wrong corrections: 4.9%
    // Crop: 20 bits, decoding success: 79.8%, wrong corrections: 2.1%
    // Crop: 21 bits, decoding success: 86.3%, wrong corrections: 1.9%
    // Crop: 22 bits, decoding success: 92.7%, wrong corrections: 0.8%
    // Crop: 23 bits, decoding success: 96.3%, wrong corrections: 0.5%
    // Crop: 24 bits, decoding success: 96.9%, wrong corrections: 0.4%
    // Crop: 25 bits, decoding success: 98.9%, wrong corrections: 0.0%
    // Crop: 26 bits, decoding success: 99.4%, wrong corrections: 0.0%
    // Crop: 27 bits, decoding success: 99.4%, wrong corrections: 0.0%
    // Crop: 28 bits, decoding success: 99.9%, wrong corrections: 0.0%
    // Crop: 29 bits, decoding success: 100.0%, wrong corrections: 0.0%
    // Crop: 30 bits, decoding success: 100.0%, wrong corrections: 0.0%

    const int num_iter = 500;
    const auto crop_size_start = 15;
    const auto crop_size_end = 30;
    std::chrono::milliseconds total_decode_time{};
    for (int crop = crop_size_start; crop <= crop_size_end; ++crop) {
        int succ = 0, fail = 0;
        for (int i = 0; i < num_iter; ++i) {
            // generate random message
            std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });
            // combine with frozen bits
            for (size_t i = 0; i < info_bits.size(); ++i)
                msg_with_frozen_bits[info_bits[i]] = msg[i];
            // encode
            auto codeword = enc.encode(msg_with_frozen_bits);
            eccpp::shuffle(codeword, shuffle_seed);

            // simulate erasures
            const auto crop_start = rg() % (N - crop + 1);
            const auto crop_end = crop_start + crop;
            for (int i = 0; i < crop_start; ++i)
                codeword[i] = -1;
            for (int i = crop_end; i < N; ++i)
                codeword[i] = -1;

            // decode
            auto llr = bits_to_llr(codeword);
            const auto start = std::chrono::steady_clock::now();
            eccpp::unshuffle(llr, shuffle_seed);
            auto result = dec.decode(llr, info_bits);
            const auto end = std::chrono::steady_clock::now();
            total_decode_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if (result == msg)
                ++succ;
            else
                ++fail;
        }

        const auto success_rate = 100.0 * succ / num_iter;
        const auto fail_rate = 100.0 * fail / num_iter;
        std::cout << "Crop size: " << crop << ", success: " << std::fixed << std::setprecision(1) << success_rate << "%, fail: " << std::fixed << std::setprecision(1) << fail_rate << "%\n";
    }
    const auto avg_decode_time = total_decode_time.count() / (num_iter * (crop_size_end - crop_size_start + 1));
    std::cout << "\nAverage codeword decode time: " << avg_decode_time << " ms\n";
}
