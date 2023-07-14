#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <string>
#include <cassert>

template<typename T>
class Matrix
{
  protected:
    std::vector<T> data_;
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;

  public:
    // Constructors and destructor
    Matrix(const std::size_t rows, const std::size_t cols, const T& value = T()) :
        data_(rows * cols, value),
        rows_(rows),
        cols_(cols)
    {}
    Matrix() = default;
    Matrix(const Matrix<T>& other) = default;
    Matrix(Matrix<T>&& other) noexcept = default;
    ~Matrix() = default;

    // Assignment
    Matrix<T>& operator=(const Matrix<T>& other) = default;
    Matrix<T>& operator=(Matrix<T>&& other) = default;

    // Set value
    inline void fill(const T& value)
    {
        std::fill(data_.begin(), data_.end(), value);
    }

    // Comparison
    inline bool operator==(const Matrix<T>& other) const { return rows_ == other.rows() && data_ == other.data_; }

    // Getters
    inline auto rows() const { return rows_; }
    inline auto cols() const { return cols_; }
    inline const T& operator()(const std::size_t i, const std::size_t j) const
    {
        assert(i < rows());
        assert(j < cols());
        return data_[i * cols_ + j];
    }

    // Setters
    inline T& operator()(const std::size_t i, const std::size_t j)
    {
        assert(i < rows());
        assert(j < cols());
        return data_[i * cols_ + j];
    }

    // Row iterators
    inline auto begin(const std::size_t row) { return data_.begin() + (row * cols()); }
    inline auto end(const std::size_t row) { return data_.begin() + ((row + 1) * cols()); }
    inline auto begin(const std::size_t row) const { return data_.begin() + (row * cols()); }
    inline auto end(const std::size_t row) const { return data_.begin() + ((row + 1) * cols()); }
    inline auto cbegin(const std::size_t row) const { return data_.cbegin() + (row * cols()); }
    inline auto cend(const std::size_t row) const { return data_.cbegin() + ((row + 1) * cols()); }

    // Print
//     void print() const
//     {
//         fmt::print("        ");
//         for (std::size_t j = 0; j < cols(); ++j)
//         {
//             fmt::print(" {:>6}", j);
//         }
//         fmt::print("\n");
//         fmt::print("       +");
//         for (std::size_t j = 0; j < cols(); ++j)
//         {
//             fmt::print("-------");
//         }
//         fmt::print("\n");
//
//         for (std::size_t i = 0; i < rows(); ++i)
//         {
//             fmt::print("{:>6} |", i);
//             for (std::size_t j = 0; j < cols(); ++j)
//             {
//                 if constexpr (std::is_same<T, bool>::value)
//                 {
//                     fmt::print(" {:>6}", static_cast<int>(operator()(i, j)));
//                 }
//                 else if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
//                 {
//                     fmt::print(" {:>6.2f}", operator()(i, j));
//                 }
//                 else
//                 {
//                     fmt::print(" {:>6}", operator()(i, j));
//                 }
//             }
//             fmt::print("\n");
//         }
//         fmt::print("\n");
//         fflush(stdout);
//     }
//     void print(const std::string* const row_names, const std::string* const col_names) const
//     {
//         fmt::print("            ");
//         for (std::size_t j = 0; j < cols(); ++j)
//         {
//             fmt::print(" {:>10}", col_names[j]);
//         }
//         fmt::print("\n");
//         fmt::print("           +");
//         for (std::size_t j = 0; j < cols(); ++j)
//         {
//             fmt::print("-----------");
//         }
//         fmt::print("\n");
//
//         for (std::size_t i = 0; i < rows(); ++i)
//         {
//             fmt::print("{:>10} |", row_names[i]);
//             for (std::size_t j = 0; j < cols(); ++j)
//             {
//                 if constexpr (std::is_same<T, bool>::value)
//                 {
//                     fmt::print(" {:>10}", static_cast<int>(operator()(i, j)));
//                 }
//                 else if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
//                 {
//                     fmt::print(" {:>10.2f}", operator()(i, j));
//                 }
//                 else
//                 {
//                     fmt::print(" {:>10}", operator()(i, j));
//                 }
//             }
//             fmt::print("\n");
//         }
//         fmt::print("\n");
//         fflush(stdout);
//     }
};

#endif
