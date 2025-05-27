#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "polar-enc.h"
#include "polar-dec.h"

#include "./shared.h"

static const struct {
    size_t N = 8192;
    // number of iterations to control execution time vs accuracy:
    // * 1 is the fastest (3-5 codewords per each case);
    // * 100 is comprehensive (can be set even higher if computation
    // time is not an issue)
    int iter_factor = 1;

    bool erasure_crop = true;
    bool erasure_scatter = true;
    bool erasure_crop_unaligned = true;
} params;

int main(int argc, char **argv) {
    const std::uint_fast32_t permutation_seed = 12345;
    eccpp::polar_enc_butterfly enc(params.N, permutation_seed);
    eccpp::polar_dec<float> dec(params.N, permutation_seed);
    std::vector<int> msg_with_frozen_bits(params.N);
    std::vector<size_t> info_bits = {4095, 6143, 7167, 7679, 7935, 8063, 8127, 8159, 8175, 8183, 8187, 8189, 8190, 8191};

    std::vector<int> msg(info_bits.size());

    // deterministic LCG for reproducible results
    std::minstd_rand rg;
    rg.seed(987654321);

    std::cout << "\nPolar encoder/decoder simulation suite\n";
    if (params.erasure_crop) {
        // "crop" simulation:
        // only a limited contiguous span of bits is available for decoding (like an image crop, hence the name).
        // An example for N = 8 and crop = 3: a codeword like {1, 1, 1, 1, 0, 0, 0, 0} turns into {_, _, _, 1, 0, 0, _, _}
        // or {1, 1, 1, _, _, _, _, _} or {_, _, _, _, _, 0, 0, 0}, etc. on the receiver side.
        std::cout << "\n# Crop-like erasure simulation:\n";

        // A test run for future reference:
        // N = 8192, rg's seed = 987654321, shuffle seed = 12345, num_iter = 500
        // info_bits[14] = {4095, 6143, 7167, 7679, 7935, 8063, 8127, 8159, 8175, 8183, 8187, 8189, 8190, 8191}
        // ------------------------------------------------------------------------------------------------------
        // Crop: 15 bits, success: 77.6%, fail: 22.4%
        // Crop: 16 bits, success: 87.8%, fail: 12.2%
        // Crop: 17 bits, success: 93.6%, fail: 6.4%
        // Crop: 18 bits, success: 98.6%, fail: 1.4%
        // Crop: 19 bits, success: 99.2%, fail: 0.8%
        // Crop: 20 bits, success: 99.0%, fail: 1.0%
        // Crop: 21 bits, success: 99.8%, fail: 0.2%
        // Crop: 22 bits, success: 100.0%, fail: 0.0%
        // Crop: 23 bits, success: 100.0%, fail: 0.0%
        // Crop: 24 bits, success: 100.0%, fail: 0.0%
        // Crop: 25 bits, success: 100.0%, fail: 0.0%
        // Crop: 26 bits, success: 100.0%, fail: 0.0%
        // Crop: 27 bits, success: 99.8%, fail: 0.2%
        // Crop: 28 bits, success: 100.0%, fail: 0.0%
        // Crop: 29 bits, success: 100.0%, fail: 0.0%
        // Crop: 30 bits, success: 100.0%, fail: 0.0%
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

        const int num_crop_iter = params.iter_factor * 10;
        const auto crop_size_start = 14;
        const auto crop_size_end = 25;
        std::chrono::milliseconds total_decode_time{};
        for (int crop = crop_size_start; crop <= crop_size_end; ++crop) {
            int succ = 0, fail = 0;
            float succ_confidence = 0;
            for (int i = 0; i < num_crop_iter; ++i) {
                // generate random message
                std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });

                // combine with frozen bits
                for (size_t i = 0; i < info_bits.size(); ++i)
                    msg_with_frozen_bits[info_bits[i]] = msg[i];
                // encode
                auto codeword = enc.encode(msg_with_frozen_bits);

                // simulate erasures: everything outside of the "crop" span is erased
                const auto crop_start = rg() % (params.N - crop + 1);
                const auto crop_end = crop_start + crop;
                for (int i = 0; i < crop_start; ++i)
                    codeword[i] = -1;
                for (int i = crop_end; i < params.N; ++i)
                    codeword[i] = -1;

                // decode
                auto llr = bits_to_llr(codeword);
                const auto start = std::chrono::steady_clock::now();
                auto result = dec.decode(llr, info_bits);
                const auto end = std::chrono::steady_clock::now();
                total_decode_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                if (result.msg == msg)
                    ++succ, succ_confidence += result.confidence;
                else
                    ++fail;
            }

            const auto success_rate = 100.0 * succ / num_crop_iter;
            const auto fail_rate = 100.0 * fail / num_crop_iter;
            const auto confidence = succ > 0 ? succ_confidence / succ : 0;
            std::cout << "Crop: " << crop << " bits, success: " << std::fixed << std::setprecision(1) << success_rate << "%, fail: " << std::setprecision(1) << fail_rate << "%, confidence: " << std::setprecision(2) << confidence << "\n";
        }
        const auto avg_decode_time = total_decode_time.count() / (num_crop_iter * (crop_size_end - crop_size_start + 1));
        std::cout << "\nAverage codeword decode time: " << avg_decode_time << " ms\n----------------------------------------\n";
    }

    if (params.erasure_scatter) {
        rg.seed(987654321);

        // "scatter" simulation:
        // scattered erasure simulation: only fixed number of randomly chosen bits are preserved.
        // {1, 1, 1, 1, 0, 0, 0, 0} -> {_, 1, _, _, 0, _, _, 0} or {1, 1, _, _, 0, _, _, _}, etc.
        std::cout << "\n# Scattered erasure simulation:\n";

        // A scattered test run, same params as before:
        // Scatter: 15 bits, success: 73.4%, fail: 26.6%
        // Scatter: 16 bits, success: 87.6%, fail: 12.4%
        // Scatter: 17 bits, success: 94.4%, fail: 5.6%
        // Scatter: 18 bits, success: 97.0%, fail: 3.0%
        // Scatter: 19 bits, success: 98.0%, fail: 2.0%
        // Scatter: 20 bits, success: 99.8%, fail: 0.2%
        // Scatter: 21 bits, success: 99.8%, fail: 0.2%
        // Scatter: 22 bits, success: 99.6%, fail: 0.4%
        // Scatter: 23 bits, success: 99.8%, fail: 0.2%
        // Scatter: 24 bits, success: 100.0%, fail: 0.0%
        // Scatter: 25 bits, success: 100.0%, fail: 0.0%
        // Scatter: 26 bits, success: 100.0%, fail: 0.0%
        // Scatter: 27 bits, success: 100.0%, fail: 0.0%
        // Scatter: 28 bits, success: 100.0%, fail: 0.0%
        // Scatter: 29 bits, success: 100.0%, fail: 0.0%
        // Scatter: 30 bits, success: 100.0%, fail: 0.0%
        // ------------------------------------------------------------------------------------------------------
        // And the 5G NR Matlab results, same params again:
        // Scatter: 15 bits, decoding success: 14.0%, wrong corrections: 10.0%, repeat code success: 0.0%
        // Scatter: 16 bits, decoding success: 25.0%, wrong corrections: 8.0%, repeat code success: 0.0%
        // Scatter: 17 bits, decoding success: 40.0%, wrong corrections: 8.0%, repeat code success: 1.0%
        // Scatter: 18 bits, decoding success: 57.0%, wrong corrections: 4.0%, repeat code success: 1.0%
        // Scatter: 19 bits, decoding success: 71.0%, wrong corrections: 4.0%, repeat code success: 2.0%
        // Scatter: 20 bits, decoding success: 82.0%, wrong corrections: 2.0%, repeat code success: 1.0%
        // Scatter: 21 bits, decoding success: 90.0%, wrong corrections: 2.0%, repeat code success: 3.0%
        // Scatter: 22 bits, decoding success: 94.0%, wrong corrections: 1.0%, repeat code success: 3.0%
        // Scatter: 23 bits, decoding success: 97.0%, wrong corrections: 1.0%, repeat code success: 5.0%
        // Scatter: 24 bits, decoding success: 98.0%, wrong corrections: 0.0%, repeat code success: 8.0%
        // Scatter: 25 bits, decoding success: 99.0%, wrong corrections: 0.0%, repeat code success: 11.0%
        // Scatter: 26 bits, decoding success: 100.0%, wrong corrections: 0.0%, repeat code success: 11.0%
        // Scatter: 27 bits, decoding success: 100.0%, wrong corrections: 0.0%, repeat code success: 16.0%
        // Scatter: 28 bits, decoding success: 100.0%, wrong corrections: 0.0%, repeat code success: 17.0%
        // Scatter: 29 bits, decoding success: 100.0%, wrong corrections: 0.0%, repeat code success: 23.0%
        // Scatter: 30 bits, decoding success: 100.0%, wrong corrections: 0.0%, repeat code success: 26.0%

        const int num_scatter_iter = params.iter_factor * 10;
        const auto scatter_size_start = 14;
        const auto scatter_size_end = 25;
        for (int num_bits = scatter_size_start; num_bits <= scatter_size_end; ++num_bits) {
            int succ = 0, fail = 0;
            float succ_confidence = 0;
            for (int i = 0; i < num_scatter_iter; ++i) {
                // generate random message
                std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });
                // combine with frozen bits
                for (size_t i = 0; i < info_bits.size(); ++i)
                    msg_with_frozen_bits[info_bits[i]] = msg[i];
                // encode
                auto codeword = enc.encode(msg_with_frozen_bits);

                // simulate erasures: mark every bit as "for-erasure" first and then unmark num_bits of them
                const int mark_shift = 5; // can be any value >= 2
                for (auto& bit: codeword)
                    bit += mark_shift;

                // careful: avoid marking the same bit multiple times. Not very likely, but still can happen
                int bits_to_mark = num_bits;
                while (bits_to_mark > 0) {
                    const size_t idx = rg() % params.N;
                    if (codeword[idx] >= mark_shift) {
                        codeword[idx] -= mark_shift;
                        --bits_to_mark;
                    }
                }

                for (auto& bit: codeword) {
                    if (bit >= mark_shift)
                        bit = -1;   // erasure
                }

                // decode
                auto llr = bits_to_llr(codeword);
                auto result = dec.decode(llr, info_bits);

                if (result.msg == msg)
                    ++succ, succ_confidence += result.confidence;
                else
                    ++fail;
            }

            const auto success_rate = 100.0 * succ / num_scatter_iter;
            const auto fail_rate = 100.0 * fail / num_scatter_iter;
            const auto confidence = succ > 0 ? succ_confidence / succ : 0;
            std::cout << "Scatter: " << num_bits << " bits, success: " << std::fixed << std::setprecision(1) << success_rate << "%, fail: " << std::setprecision(1) << fail_rate << "%, confidence: " << std::setprecision(2) << confidence << "\n";
        }
        std::cout << "----------------------------------------\n";
    }

    if (params.erasure_crop_unaligned) {
        std::cout << "\n# Crop-like erasure simulation with unknown crop location:\n";

        // this is a lot slower, no x10 factor
        const int num_crop_iter = params.iter_factor;
        const auto crop_size_start = 25;
        const auto crop_size_end = 35;
        std::chrono::milliseconds total_decode_time{};
        for (int crop = crop_size_start; crop <= crop_size_end; ++crop) {
            int succ = 0, fail = 0;
            float succ_confidence = 0;
            for (int i = 0; i < num_crop_iter; ++i) {
                // generate random message
                std::generate(msg.begin(), msg.end(), [&rg]() { return rg() & 1; });

                // combine with frozen bits
                for (size_t i = 0; i < info_bits.size(); ++i)
                    msg_with_frozen_bits[info_bits[i]] = msg[i];
                // encode
                auto codeword = enc.encode(msg_with_frozen_bits);

                // simulate erasures: everything outside of the "crop" span is erased
                const auto crop_start = rg() % (params.N - crop + 1);
                const auto crop_end = crop_start + crop;
                std::vector<int> unaligned_codeword(codeword.begin() + crop_start, codeword.begin() + crop_end);

                // decode
                auto llr = bits_to_llr(unaligned_codeword);
                const auto start = std::chrono::steady_clock::now();
                auto result = dec.decode_unaligned(llr, info_bits);
                const auto end = std::chrono::steady_clock::now();
                total_decode_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                if (result.msg == msg)
                    ++succ, succ_confidence += result.confidence;
                else
                    ++fail;
            }

            const auto success_rate = 100.0 * succ / num_crop_iter;
            const auto fail_rate = 100.0 * fail / num_crop_iter;
            const auto confidence = succ > 0 ? succ_confidence / succ : 0;
            std::cout << "Crop: " << crop << " bits, success: " << std::fixed << std::setprecision(1) << success_rate << "%, fail: " << std::setprecision(1) << fail_rate << "%, confidence: " << std::setprecision(2) << confidence << "\n";
        }
        const auto avg_decode_time = total_decode_time.count() / (num_crop_iter * (crop_size_end - crop_size_start + 1));
        std::cout << "\nAverage codeword decode time: " << avg_decode_time << " ms\n----------------------------------------\n";
    }

    std::cout << "\nDone\n";
}
