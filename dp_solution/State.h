#pragma once
#include "DynamicBitset.h"
#include <iostream>

struct State {
  State();
  State(std::string name, int time, double load, DynamicBitset seen, double cost);
  State(const State &other);

  bool has_been_to(size_t node) const;
  bool operator<(const State other) const;
  bool operator>(const State other) const;
  bool operator==(const State other) const;

  std::string node;
  int time;
  int64_t hash;
  double load;
  double cost;
  DynamicBitset nodes_seen;
};

template <> struct std::hash<State> {
  std::size_t operator()(const State &k) const;
};