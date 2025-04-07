#ifndef ECCPP_MDARRAY_H
#define ECCPP_MDARRAY_H

#include <vector>
#include <stdexcept>
#include <iostream>

namespace eccpp {

template <typename T>
class mdarray {
    std::vector<size_t> dims_;
    std::vector<size_t> index_stride_;
    std::vector<T> data_;

public:
    mdarray(const std::vector<size_t>& dims): dims_(dims) {
        if (dims.empty())
            throw std::invalid_argument("No dimensions specified");

        size_t total = 1;
        for (auto d: dims) {
            if (!d)
                throw std::invalid_argument("Invalid zero-size dimension");

            // just in case someone tries to create a gigantic mdarray
            // with the total size not fitting into size_t
            const auto total_prev = total;
            total *= d;
            if ((total / d) != total_prev)
                throw std::invalid_argument("Dimensions too large");
        }
        data_ = std::vector<T>(total, T());

        // pre-calculate strides/offsets for faster indexing
        index_stride_.reserve(dims.size());
        for (auto d: dims) {
            total /= d;
            index_stride_.push_back(total);
        }
    }

    const std::vector<size_t>& dimensions() const { return dims_; }

    // access with multi-index
    T& operator()(const std::vector<size_t>& indices) {
        return data_[offset(indices)];
    }

    const T& operator()(const std::vector<size_t>& indices) const {
        return data_[offset(indices)];
    }

    friend std::ostream& operator<<(std::ostream& os, const mdarray<T>& arr) {
        const auto& dims = arr.dimensions();
        if (dims.size() == 1) {
           os << "[";
            for (size_t i = 0; i < dims[0]; ++i) {
                if (i > 0)
                    os << ", ";

                os << arr({i});
            }
            os << "]\n";
        }
        else if (dims.size() == 2) {
            for (size_t i = 0; i < dims[0]; ++i) {
                os << "[";
                for (size_t j = 0; j < dims[1]; ++j) {
                    if (j > 0)
                        os << ", ";

                    os << arr({i, j});
                }
                os << "]\n";
            }
        }

        return os;
    }

private:
    size_t offset(const std::vector<size_t>& indices) const {
        if (indices.size() != dims_.size())
            throw std::invalid_argument("Number of indices does not match number of dimensions");

        size_t off = 0;
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] >= dims_[i])
                throw std::invalid_argument("Index out of bounds");

            off += indices[i] * index_stride_[i];
        }

        return off;
    }
};

} // namespace eccpp

#endif // ECCPP_MDARRAY_H
