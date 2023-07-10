#include "State.h"
#include <sstream>
#include <tuple>

#define USE_HASH

// Taken from boost::hash_combine: combines two hash values
// (https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine)

// template <class T> inline void hash_combine(int64_t &seed, const T &v) {
//   std::hash<T> hasher;
//   seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
// }
