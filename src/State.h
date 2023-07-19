#pragma once
#include <iostream>

template <typename IntType> struct State {
  State() {}
  State(int node, int time, int16_t load, IntType seen, double cost, int index_in_prev)
      : nodes_seen(seen), node(node), time(time), load(load), cost(cost),
        index_in_prev(index_in_prev) {}
  // State(int node, int time, double load, IntType seen, double cost, int index_in_prev)
  //     : nodes_seen(seen), node(node), time(time), load(load), cost(cost),
  //       index_in_prev(index_in_prev) {
  //   this->node = node;
  //   this->time = time;
  //   this->load = load;
  //   this->cost = cost;
  //   this->index_in_prev = index_in_prev;
  // }
  State(const State &other) = default;

  bool has_been_to(size_t node) const { return nodes_seen & (((IntType)1) << node); }
  bool operator>(const State other) const {
    //
    return std::tie(time, node, load) > std::tie(other.time, other.node, other.load);
  };
  bool operator==(const State<IntType> &other) const = default;
  bool dominates(const State other) const;

  std::string to_string() const {
    return std::to_string(node) + " " + std::to_string(time) + ", cost " + std::to_string(cost)
           + " load " + std::to_string(load);
  };

  int node;
  int time;
  int16_t load;
  double cost;
  int index_in_prev;
  IntType nodes_seen;
};