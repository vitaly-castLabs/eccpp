#ifndef ECCPP_H
#define ECCPP_H

#include <vector>

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
};

}

#endif // #ifndef ECCPP_H
