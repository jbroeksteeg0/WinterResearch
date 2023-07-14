#ifndef CONTIGUOUS_LABEL_POOL_H
#define CONTIGUOUS_LABEL_POOL_H

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>

#define MIN_CAPACITY (1000)

struct Label;

class ContiguousLabelPool
{
    std::byte* data_{nullptr};
    std::size_t container_size_{0};
    std::size_t container_capacity_{0};
    std::size_t label_size_{0};

  public:
    // Constructors and destructor
    ContiguousLabelPool() = default;
    ContiguousLabelPool(const ContiguousLabelPool&) = delete;
    ContiguousLabelPool(ContiguousLabelPool&&) = delete;
    ContiguousLabelPool& operator=(const ContiguousLabelPool&) = delete;
    ContiguousLabelPool& operator=(ContiguousLabelPool&&) = delete;
    ~ContiguousLabelPool() { std::free(data_); }

    // Getters
    inline auto label_size() const { return label_size_; }
    inline auto size() const { return container_size_ / label_size_; }
    inline Label* operator[](const std::size_t idx) const
    {
        assert(container_size_ >= label_size_ * (idx + 1));
        return reinterpret_cast<Label*>(data_ + label_size_ * idx);
    }

    // Get some memory to write a temporary label
    Label* get_label_buffer()
    {
        // Reallocate.
        if (container_size_ + label_size_ > container_capacity_)
        {
            container_capacity_ *= 2; // TODO: Use a better growth factor
            data_ = reinterpret_cast<std::byte*>(std::realloc(data_, container_capacity_));
            assert(data_); // WARNING: This assert is not active in release builds
        }

        // Return memory address.
        return reinterpret_cast<Label*>(data_ + container_size_);
    }

    // Commit the temporary label to the container
    inline void commit_label_in_buffer()
    {
        container_size_ += label_size_;
    }

    // Resize the container
    inline void resize(const std::size_t size)
    {
        container_size_ = size * label_size_;
    }

    // Clear the container and set a new label size
    void reset(const std::size_t label_size)
    {
        // Ensure that the label size is a multiple of 8.
        assert(reinterpret_cast<uintptr_t>(label_size) % CHAR_BIT == 0);
        label_size_ = label_size;
        // label_size_ = label_size + (CHAR_BIT - label_size) % CHAR_BIT; // Round up to next multiple of 8

        // Reset pointer to the start of the container and reallocate if too small.
        container_size_ = 0;
        if (const auto min_capacity = label_size_ * MIN_CAPACITY; container_capacity_ < min_capacity)
        {
            container_capacity_ = min_capacity;
            data_ = reinterpret_cast<std::byte*>(std::realloc(data_, container_capacity_));
            assert(data_); // WARNING: This assert is not active in release builds
        }
    }
};

#endif
