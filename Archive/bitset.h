#ifndef BITSET_H
#define BITSET_H

#include <cstddef>
#include <climits>

inline bool get_bitset(const void* const bitset, const std::size_t i)
{
    const size_t idx = i / CHAR_BIT;
    const unsigned char mask = 1 << (i % CHAR_BIT);
    return (reinterpret_cast<const unsigned char*>(bitset)[idx] & mask) != 0;
}

inline void set_bitset(void* const bitset, const std::size_t i)
{
    const size_t idx = i / CHAR_BIT;
    const unsigned char mask = 1 << (i % CHAR_BIT);
    reinterpret_cast<unsigned char*>(bitset)[idx] |= mask;
}

inline void clear_bitset(void* const bitset, const std::size_t i)
{
    const size_t idx = i / CHAR_BIT;
    const unsigned char mask = 1 << (i % CHAR_BIT);
    reinterpret_cast<unsigned char*>(bitset)[idx] &= ~mask;
}

inline bool flip_bitset(void* const bitset, const std::size_t i)
{
    const size_t idx = i / CHAR_BIT;
    const unsigned char mask = 1 << (i % CHAR_BIT);
    reinterpret_cast<unsigned char*>(bitset)[idx] ^= mask;
    return (reinterpret_cast<unsigned char*>(bitset)[idx] & mask) != 0;
}

#endif
