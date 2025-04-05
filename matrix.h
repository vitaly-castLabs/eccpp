#ifndef ECCPP_MATRIX_H
#define ECCPP_MATRIX_H

#include <vector>
#include <iostream>

namespace eccpp {

template <typename T>
class matrix {
private:
    std::vector<std::vector<T>> data;

public:
    matrix(size_t rows, size_t cols) : data(rows, std::vector<T>(cols)) {}
    
    matrix(const std::vector<std::vector<T>>& input) : data(input) {}
    
    size_t rows() const { return data.size(); }
    size_t cols() const { return data.empty() ? 0 : data[0].size(); }
    
    const std::vector<T>& operator[](size_t i) const { return data[i]; }
    std::vector<T>& operator[](size_t i) { return data[i]; }

    friend std::ostream& operator<<(std::ostream& os, const matrix<T>& mat) {
        for (size_t i = 0; i < mat.rows(); ++i) {
            os << "[";
            for (size_t j = 0; j < mat.cols(); ++j) {
                os << mat[i][j];
                if (j < mat.cols() - 1)
                    os << ", ";
            }
            os << "]\n";
        }
        return os;
    }
};

}

#endif // #ifndef ECCPP_MATRIX_H
