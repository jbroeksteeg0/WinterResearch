#pragma once

#include <iostream>
#include <map>
#include <vector>

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

  void add_edge(std::string a, std::string b, int distance);

  double get_distance(std::string a, std::string b) const;
  double get_cost(std::string a, std::string b) const;
  int get_num_nodes() const;
  int get_node_unpack_time(const std::string &name) const;
  double get_node_load(const std::string &name) const;
  std::pair<int, int> get_node_times(const std::string &name) const;
  bool node_exists(const std::string &name) const;
  std::vector<std::string> get_node_names() const;
  int get_node_ind(const std::string &name) const;
  Node get_node_data(const std::string &name) const;

private:
  std::map<std::string, Node> m_nodes;
  std::map<std::pair<std::string, std::string>, double> m_dist;
};