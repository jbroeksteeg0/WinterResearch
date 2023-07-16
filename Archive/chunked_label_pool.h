#ifndef CHUNKED_LABEL_POOL_H
#define CHUNKED_LABEL_POOL_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>

#define BLOCK_SIZE (10 * 1024 * 1024)
#define RESERVE_BLOCKS (50)

struct Label;

class ChunkedLabelPool
{
    std::vector<std::unique_ptr<std::byte[]>> blocks_;
    std::size_t block_idx_;
    std::size_t byte_idx_;
    std::size_t label_size_;

  public:
    // Constructors and destructor
    ChunkedLabelPool(): blocks_(), block_idx_(0), byte_idx_(0), label_size_(1)
    {
        static_assert(BLOCK_SIZE % CHAR_BIT == 0);

        blocks_.reserve(RESERVE_BLOCKS);
        blocks_.push_back(std::make_unique<std::byte[]>(BLOCK_SIZE));
        assert(blocks_.back());
    }
    ChunkedLabelPool(const ChunkedLabelPool&) = delete;
    ChunkedLabelPool(ChunkedLabelPool&&) = delete;
    ChunkedLabelPool& operator=(const ChunkedLabelPool&) = delete;
    ChunkedLabelPool& operator=(ChunkedLabelPool&&) = delete;
    ~ChunkedLabelPool() = default;

    // Getters
    inline auto label_size() const { return label_size_; }

    // Get some memory to write a temporary label
    Label* get_label_buffer()
    {
        // Advance to the next block if there's no space in the current block.
        if (byte_idx_ + label_size_ >= BLOCK_SIZE)
        {
            block_idx_++;
            byte_idx_ = 0;
        }

        // Allocate new block if no space left.
        if (block_idx_ == static_cast<std::size_t>(blocks_.size()))
        {
            blocks_.push_back(std::make_unique<std::byte[]>(BLOCK_SIZE));
            assert(blocks_.back());
        }

        // Get the address to store the label.
        assert(block_idx_ < blocks_.size());
        assert(byte_idx_ < BLOCK_SIZE);
        auto label = reinterpret_cast<Label*>(&(blocks_[block_idx_][byte_idx_]));
        assert(reinterpret_cast<uintptr_t>(label) % CHAR_BIT == 0);

        // Done.
        return label;
    }

    // Commit the temporary label to the container
    void commit_label_in_buffer()
    {
        assert(block_idx_ < static_cast<std::size_t>(blocks_.size()));
        assert(byte_idx_ < BLOCK_SIZE);
        assert(label_size_ % CHAR_BIT == 0);
        byte_idx_ += label_size_;
    }

    // Clear the container and set a new label size
    void reset(const std::size_t label_size)
    {
        // Reset pointers to the start of the first block.
        block_idx_ = 0;
        byte_idx_ = 0;

        // Ensure that the label size is a multiple of 8.
        assert(reinterpret_cast<uintptr_t>(label_size) % CHAR_BIT == 0);
        label_size_ = label_size;
        // label_size_ = label_size + (CHAR_BIT - label_size) % CHAR_BIT; // Round up to next multiple of 8
    }
};

#endif
