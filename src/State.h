#pragma once
#include "DynamicBitset.h"
#include <iostream>

struct State {
  State();
  State(int node, int time, double load, __int128 seen, double cost);
  State(const State &other) = default;

  bool has_been_to(size_t node) const;
  bool operator<(const State other) const { return time >= other.time; };
  bool operator>(const State other) const;
  bool operator==(const State other) const;
  bool dominates(const State other) const;

  std::string to_string() const;

  int node;
  int time;
  int64_t hash;
  double load;
  double cost;
  __int128 nodes_seen;
};