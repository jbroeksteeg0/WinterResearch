#include "State.h"
#include "DynamicBitset.h"
#include <tuple>

#define USE_HASH

// Taken from boost::hash_combine: combines two hash values
// (https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine)

template <class T> inline void hash_combine(int64_t &seed, const T &v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

State::State(std::string node, int time, double load, DynamicBitset seen, double cost)
    : nodes_seen(seen) {
  this->node = node;
  this->time = time;
  this->load = load;
  this->cost = cost;

#ifdef USE_HASH
  hash = 0;
  // Calculate hash value:
  for (char ch : node) {
    hash_combine(hash, ch);
  }
  hash_combine(hash, time);
  hash_combine(hash, load);
  for (int i = 0; i < 16; i++)
    hash_combine(hash, seen.m_elems[i]);
#endif
}

State::State(const State &other) : nodes_seen(other.nodes_seen) {
  this->node = other.node;
  this->time = other.time;
  this->load = other.load;
  this->cost = other.cost;
  this->hash = other.hash;
}
State::State() : nodes_seen(1) {
  std::cout << "Default called" << std::endl;
  exit(1);
}
bool State::has_been_to(size_t node) const { return nodes_seen.get(node); }

bool State::operator==(const State other) const {
  return std::tie(
           cost,
           node,
           time,
           load,
           nodes_seen.m_elems[0],
           nodes_seen.m_elems[1],
           nodes_seen.m_elems[2],
           nodes_seen.m_elems[3],
           nodes_seen.m_elems[4],
           nodes_seen.m_elems[5],
           nodes_seen.m_elems[6],
           nodes_seen.m_elems[7],
           nodes_seen.m_elems[8],
           nodes_seen.m_elems[9],
           nodes_seen.m_elems[10],
           nodes_seen.m_elems[11],
           nodes_seen.m_elems[12],
           nodes_seen.m_elems[13],
           nodes_seen.m_elems[14],
           nodes_seen.m_elems[15]
         )
         == std::tie(
           other.cost,
           other.node,
           other.time,
           other.load,
           other.nodes_seen.m_elems[0],
           other.nodes_seen.m_elems[1],
           other.nodes_seen.m_elems[2],
           other.nodes_seen.m_elems[3],
           other.nodes_seen.m_elems[4],
           other.nodes_seen.m_elems[5],
           other.nodes_seen.m_elems[6],
           other.nodes_seen.m_elems[7],
           other.nodes_seen.m_elems[8],
           other.nodes_seen.m_elems[9],
           other.nodes_seen.m_elems[10],
           other.nodes_seen.m_elems[11],
           other.nodes_seen.m_elems[12],
           other.nodes_seen.m_elems[13],
           other.nodes_seen.m_elems[14],
           other.nodes_seen.m_elems[15]
         );
}
// USED BY SEEN, CANNOT USE COST
bool State::operator<(const State other) const {
#ifdef USE_HASH
  return hash < other.hash;
#endif
  return std::tie(
           node,
           time,
           load,
           nodes_seen.m_elems[0],
           nodes_seen.m_elems[1],
           nodes_seen.m_elems[2],
           nodes_seen.m_elems[3],
           nodes_seen.m_elems[4],
           nodes_seen.m_elems[5],
           nodes_seen.m_elems[6],
           nodes_seen.m_elems[7],
           nodes_seen.m_elems[8],
           nodes_seen.m_elems[9],
           nodes_seen.m_elems[10],
           nodes_seen.m_elems[11],
           nodes_seen.m_elems[12],
           nodes_seen.m_elems[13],
           nodes_seen.m_elems[14],
           nodes_seen.m_elems[15]
         )
         < std::tie(
           other.node,
           other.time,
           other.load,
           other.nodes_seen.m_elems[0],
           other.nodes_seen.m_elems[1],
           other.nodes_seen.m_elems[2],
           other.nodes_seen.m_elems[3],
           other.nodes_seen.m_elems[4],
           other.nodes_seen.m_elems[5],
           other.nodes_seen.m_elems[6],
           other.nodes_seen.m_elems[7],
           other.nodes_seen.m_elems[8],
           other.nodes_seen.m_elems[9],
           other.nodes_seen.m_elems[10],
           other.nodes_seen.m_elems[11],
           other.nodes_seen.m_elems[12],
           other.nodes_seen.m_elems[13],
           other.nodes_seen.m_elems[14],
           other.nodes_seen.m_elems[15]
         );
}

// USED BY PQ, MUST USE COST
bool State::operator>(const State other) const {
#ifdef USE_HASH
  if (cost == other.cost)
    return hash > other.hash;
  return cost > other.cost;
#endif
  return std::tie(
           cost,
           node,
           time,
           load,
           nodes_seen.m_elems[0],
           nodes_seen.m_elems[1],
           nodes_seen.m_elems[2],
           nodes_seen.m_elems[3],
           nodes_seen.m_elems[4],
           nodes_seen.m_elems[5],
           nodes_seen.m_elems[6],
           nodes_seen.m_elems[7],
           nodes_seen.m_elems[8],
           nodes_seen.m_elems[9],
           nodes_seen.m_elems[10],
           nodes_seen.m_elems[11],
           nodes_seen.m_elems[12],
           nodes_seen.m_elems[13],
           nodes_seen.m_elems[14],
           nodes_seen.m_elems[15]
         )
         > std::tie(
           other.cost,
           other.node,
           other.time,
           other.load,
           other.nodes_seen.m_elems[0],
           other.nodes_seen.m_elems[1],
           other.nodes_seen.m_elems[2],
           other.nodes_seen.m_elems[3],
           other.nodes_seen.m_elems[4],
           other.nodes_seen.m_elems[5],
           other.nodes_seen.m_elems[6],
           other.nodes_seen.m_elems[7],
           other.nodes_seen.m_elems[8],
           other.nodes_seen.m_elems[9],
           other.nodes_seen.m_elems[10],
           other.nodes_seen.m_elems[11],
           other.nodes_seen.m_elems[12],
           other.nodes_seen.m_elems[13],
           other.nodes_seen.m_elems[14],
           other.nodes_seen.m_elems[15]
         );
}

std::size_t std::hash<State>::operator()(const State &k) const { return k.hash; }