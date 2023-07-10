#pragma once
#include <iostream>

template <typename IntType> struct State {
  State();
  State(int node, int time, double load, IntType seen, double cost) : nodes_seen(seen) {
    this->node = node;
    this->time = time;
    this->load = load;
    this->cost = cost;
  }
  State(const State &other) = default;

  bool has_been_to(size_t node) const { return nodes_seen & (((IntType)1) << node); }
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
  IntType nodes_seen;
};