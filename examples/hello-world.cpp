// the most basic polar encoding and decoding example

#include <iostream>

#include "polar-enc.h"
#include "polar-dec.h"

#include "./shared.h"

constexpr size_t N = 16;
const std::vector<size_t> info_bits = {14, 15};

void printBits(const std::vector<int>& bits) {
    for (const auto& b: bits)
        std::cout << (b ? '1' : '0');
}

std::vector<int> encodeMessage(const eccpp::polar_enc_butterfly& enc, const std::vector<int>& msg) {
    std::vector<int> msg_with_frozen_bits(N);
    for (auto i = 0; i < info_bits.size(); ++i)
        msg_with_frozen_bits[info_bits[i]] = msg[i];

    return enc.encode(msg_with_frozen_bits);
}

std::vector<float> transmitMessage(const std::vector<int>& codeword) {
    auto llr = bits_to_llr(codeword);

    // simulate earsure of the first bit
    llr[0] = 0;

    return llr;
}

std::vector<int> decodeMessage(const eccpp::polar_dec<float>& dec, const std::vector<float>& llr) {
    const auto dec_result = dec.decode(llr, info_bits);
    return dec_result.msg;
}

void runFullMessageCycle(const eccpp::polar_enc_butterfly& enc, const eccpp::polar_dec<float>& dec, const std::vector<int>& msg) {
    const auto codeword = encodeMessage(enc, msg);
    const auto llr = transmitMessage(codeword);
    const auto decoded_msg = decodeMessage(dec, llr);

    printBits(msg);
    std::cout << " -> ";
    printBits(codeword);
    std::cout << " -> ";
    printBits(decoded_msg);
    std::cout << '\n';
}

int main() {
    eccpp::polar_enc_butterfly enc(N);
    eccpp::polar_dec<float> dec(N);

    runFullMessageCycle(enc, dec, {0, 0});
    runFullMessageCycle(enc, dec, {0, 1});
    runFullMessageCycle(enc, dec, {1, 0});
    runFullMessageCycle(enc, dec, {1, 1});
}
