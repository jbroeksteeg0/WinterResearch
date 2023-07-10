#include "State.h"
#include "DynamicBitset.h"
#include <sstream>
#include <tuple>

#define USE_HASH

// Taken from boost::hash_combine: combines two hash values
// (https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine)

// template <class T> inline void hash_combine(int64_t &seed, const T &v) {
//   std::hash<T> hasher;
//   seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
// }

State::State(int node, int time, double load, __int128 seen, double cost) : nodes_seen(seen) {
  this->node = node;
  this->time = time;
  this->load = load;
  this->cost = cost;

  // #ifdef USE_HASH
  //   hash = 0;
  //   // Calculate hash value:
  //   for (char ch : node) {
  //     hash_combine(hash, ch);
  //   }
  //   hash_combine(hash, time);
  //   hash_combine(hash, load);
  //   for (int i = 0; i < 16; i++)
  //     hash_combine(hash, seen.m_elems[i]);
  // #endif
}

bool State::has_been_to(size_t node) const { return nodes_seen & (((__int128)1) << node); }