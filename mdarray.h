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

    // equality operators
    friend bool operator==(const mdarray<T>& lhs, const mdarray<T>& rhs) {
        return (lhs.dims_ == rhs.dims_) && (lhs.data_ == rhs.data_);
    }
    friend bool operator!=(const mdarray<T>& lhs, const mdarray<T>& rhs) {
        return !(lhs == rhs);
    }

    // matrix multiplication / tensor contraction
    mdarray<T> operator*(const mdarray<T>& rhs) const {
        auto& lhs = *this;

        const auto& lhs_dims = lhs.dimensions();
        const auto& rhs_dims = rhs.dimensions();

        if (lhs_dims.empty() || rhs_dims.empty())
            throw std::invalid_argument("Cannot contract empty mdarray");

        // contract last dimension of lhs with first dimension of rhs
        const size_t contracted_dim = lhs_dims.back();
        if (rhs_dims.front() != contracted_dim)
            throw std::invalid_argument("Dimension mismatch for contraction");

        // compute new dimensions: (lhs_dims[0..n - 2] + rhs_dims[1..m])
        std::vector<size_t> new_dims;
        new_dims.reserve(lhs_dims.size() + rhs_dims.size() - 2);
        new_dims.insert(new_dims.end(), lhs_dims.begin(), lhs_dims.end() - 1);
        new_dims.insert(new_dims.end(), rhs_dims.begin() + 1, rhs_dims.end());

        mdarray<T> result(new_dims);
        std::vector<size_t> indices(new_dims.size(), 0);
        while (true) {
            // compute the sum over the contracted dimension
            T sum = 0;
            for (size_t m = 0; m < contracted_dim; ++m) {
                // build left indices: [indices[0..lhs_rank - 2], m]
                std::vector<size_t> left_indices(lhs_dims.size() - 1);
                for (size_t i = 0; i < lhs_dims.size() - 1; ++i)
                    left_indices[i] = indices[i];

                left_indices.push_back(m);

                // build right indices: [m, indices[lhs_rank - 1..]]
                std::vector<size_t> right_indices(rhs_dims.size());
                right_indices[0] = m;
                for (size_t i = 1; i < rhs_dims.size(); ++i)
                    right_indices[i] = indices[lhs_dims.size() - 1 + i - 1];

                sum += lhs(left_indices) * rhs(right_indices);
            }

            // assign to result
            result(indices) = sum;

            // increment indices (row-major order)
            bool done = true;
            for (int i = indices.size() - 1; i >= 0; --i) {
                if (++indices[i] < new_dims[i]) {
                    done = false;
                    break;
                }
                indices[i] = 0;
            }
            if (done)
                break;
        }

        return result;
    }

    mdarray<T>& operator*=(const mdarray<T>& rhs) {
        *this = *this * rhs;
        return *this;
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
