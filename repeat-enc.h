// repeat code generators:
// a) bit-level repeat: Rep(3, 10) = 111000;
// b) message-level repeat: Rep(3, 10) = 101010;

#ifndef ECCPP_REPEAT_ENC_H
#define ECCPP_REPEAT_ENC_H

#include <vector>

namespace eccpp {

class repeat_enc {
public:
    repeat_enc(size_t r): r_(r) {}

    virtual ~repeat_enc() = default;

    virtual std::vector<int> encode(const std::vector<int>& data) const = 0;

protected:
    size_t r_;
};

class repeat_bit_enc: public repeat_enc {
public:
    repeat_bit_enc(size_t r): repeat_enc(r) {}

    std::vector<int> encode(const std::vector<int>& data) const override {
        const auto N = data.size();
        if (!N || !r_)
            return {};

        std::vector<int> result(N * r_);
        for (size_t i = 0; i < N; ++i) {
            const auto bit = data[i];
            const auto off = i * r_;
            for (size_t j = 0; j < r_; ++j)
                result[off + j] = bit;
        }
        return result;
    }
};

class repeat_msg_enc: public repeat_enc {
public:
    repeat_msg_enc(size_t r): repeat_enc(r) {}

    std::vector<int> encode(const std::vector<int>& data) const override {
        const auto N = data.size();
        if (!N || !r_)
            return {};

        std::vector<int> result(N * r_);
        for (size_t i = 0; i < r_; ++i) {
            const auto off = i * N;
            for (size_t j = 0; j < N; ++j)
                result[off + j] = data[j];
        }
        return result;
    }
};

} // namespace eccpp

#endif // ECCPP_REPEAT_ENC_H
