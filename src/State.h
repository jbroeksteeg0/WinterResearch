#pragma once
#include "DynamicBitset.h"
#include <iostream>

struct State {
  State();
  State(int node, int time, double load, DynamicBitset seen, double cost);
  State(const State &other) = default;

  bool has_been_to(size_t node) const;
  bool operator<(const State other) const;
  bool operator>(const State other) const;
  bool operator==(const State other) const;
  bool dominates(const State other) const;

  std::string to_string() const;

  int node;
  int time;
  int64_t hash;
  double load;
  double cost;
  DynamicBitset nodes_seen;
};