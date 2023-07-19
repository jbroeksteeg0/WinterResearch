#pragma once
#include <iostream>

template <typename IntType> struct State {
  State() {}
  State(int node, int16_t time, int16_t load, IntType seen, double cost, int index_in_prev)
      : nodes_seen(seen), node(node), time(time), load(load), cost(cost),
        index_in_prev(index_in_prev) {}
  State(const State &other) = default;

  bool has_been_to(size_t node) const { return nodes_seen & (((IntType)1) << node); }
  bool operator>(const State other) const {
    //
    return std::tie(time, node, load) > std::tie(other.time, other.node, other.load);
  };

  int node;
  int16_t time;
  int16_t load;
  double cost;
  int index_in_prev;
  IntType nodes_seen;
};