#ifndef ECCPP_MATRIX_H
#define ECCPP_MATRIX_H

#include <vector>
#include <iostream>

namespace eccpp {

template <typename T, size_t Rows, size_t Cols>
class matrix {
private:
    T data[Rows][Cols];
    
public:
    matrix() {
        for (size_t r = 0; r < Rows; ++r)
            for (size_t c = 0; c < Cols; ++c)
                data[r][c] = T();
    }
    
    matrix(const T (&input)[Rows][Cols]) {
        for (size_t r = 0; r < Rows; ++r)
            for (size_t c = 0; c < Cols; ++c)
                data[r][c] = input[r][c];
    }
    
    matrix(const std::initializer_list<std::initializer_list<T>>& input) {
        size_t r = 0;
        for (const auto& row: input) {
            if (r >= Rows) break;
            
            size_t c = 0;
            for (const auto& val: row) {
                if (c >= Cols) break;
                data[r][c] = val;
                ++c;
            }
            
            for (size_t c2 = c; c2 < Cols; ++c2)
                data[r][c2] = T();
            
            ++r;
        }
        
        for (size_t r2 = r; r2 < Rows; ++r2)
            for (size_t c = 0; c < Cols; ++c)
                data[r2][c] = T();
    }
    
    static constexpr size_t rows() { return Rows; }
    static constexpr size_t cols() { return Cols; }
    
    const T* operator[](size_t r) const { return data[r]; }
    T* operator[](size_t r) { return data[r]; }
    
    // matrix + matrix
    matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& other) const {
        matrix<T, Rows, Cols> result;
        for (size_t r = 0; r < Rows; ++r)
            for (size_t c = 0; c < Cols; ++c)
                result[r][c] = data[r][c] + other[r][c];

        return result;
    }
    matrix<T, Rows, Cols>& operator+=(const matrix<T, Rows, Cols>& other) {
        for (size_t r = 0; r < Rows; ++r)
            for (size_t c = 0; c < Cols; ++c)
                data[r][c] += other[r][c];

        return *this;
    }
    
    // matrix x matrix (matrix multiplication, NOT Hadamard/element-wise product)
    template <size_t OtherCols>
    matrix<T, Rows, OtherCols> operator*(const matrix<T, Cols, OtherCols>& other) const {
        matrix<T, Rows, OtherCols> result;
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < OtherCols; ++c) {
                auto acc = T();
                for (size_t k = 0; k < Cols; ++k)
                    acc += data[r][k] * other[k][c];

                result[r][c] = acc;
            }
        }
        return result;
    }
    matrix<T, Rows, Cols>& operator*=(const matrix<T, Cols, Cols>& other) {
        *this = *this * other;
        return *this;
    }
    
    // matrix x scalar
    matrix<T, Rows, Cols> operator*(const T& scalar) const {
        matrix<T, Rows, Cols> result;
        for (size_t r = 0; r < Rows; ++r)
            for (size_t c = 0; c < Cols; ++c)
                result[r][c] = data[r][c] * scalar;

        return result;
    }    
    matrix<T, Rows, Cols>& operator*=(const T& scalar) {
        for (size_t r = 0; r < Rows; ++r) 
            for (size_t c = 0; c < Cols; ++c)
                data[r][c] *= scalar;

        return *this;
    }
    friend matrix<T, Rows, Cols> operator*(const T& scalar, const matrix<T, Rows, Cols>& mat) {
        return mat * scalar;
    }
    
    // std::cout / cerr output
    friend std::ostream& operator<<(std::ostream& os, const matrix<T, Rows, Cols>& mat) {
        for (size_t r = 0; r < Rows; ++r) {
            os << "[";
            for (size_t c = 0; c < Cols; ++c) {
                os << mat[r][c];
                if (c < Cols - 1)
                    os << ", ";
            }
            os << "]\n";
        }
        return os;
    }
    
    // to be avoided when working with float/double matrices
    bool operator==(const matrix<T, Rows, Cols>& other) const {
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < Cols; ++c) {
                if (data[r][c] != other[r][c])
                    return false;
            }
        }
        return true;
    }
};

}

#endif // #ifndef ECCPP_MATRIX_H
