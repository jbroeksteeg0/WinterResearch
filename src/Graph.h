#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define NUM_NODES 102

struct Node {
  int x, y;
  std::pair<int, int> open_times;
  int unload_time;
  double load;
  double bias;
  int index;
  Node(){};
  Node(
    int x,
    int y,
    std::pair<int, int> open_times,
    int unload_time,
    double load,
    double bias,
    int index
  );
  std::string to_string() const {
    return "Node (" + std::to_string(x) + "," + std::to_string(y) + "), " + std::to_string(index)
           + ": " + std::to_string(load) + " load, " + std::to_string(bias) + " bias, unload time "
           + std::to_string(unload_time);
  }
};

class Graph {
public:
  Graph();

  void add_node(
    std::string name,
    std::pair<int, int> position,
    std::pair<int, int> times,
    int unload_time,
    double load,
    double bias,
    int index
  );

  void add_edge(int a, int b, int distance);

  double get_distance(int a, int b) const;
  double get_cost(int a, int b) const;
  int get_num_nodes() const;
  int get_node_unpack_time(int name) const;
  double get_node_load(int name) const;
  std::pair<int, int> get_node_times(int name) const;
  std::vector<std::string> get_node_names() const;
  int get_node_ind(int name) const;
  Node get_node_data(int name) const;

  std::array<Node, NUM_NODES + 1> m_nodes_;
  std::array<std::array<int, NUM_NODES + 1>, NUM_NODES + 1> m_dist_;

private:
  std::vector<std::string> m_node_names;
};