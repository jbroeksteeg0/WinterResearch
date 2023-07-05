#include "Graph.h"
#include <algorithm>
#include <cassert>

Node::Node(
  int x, int y, std::pair<int, int> open_times, int unload_time, double load, double bias, int index
) {
  this->x = x;
  this->y = y;
  this->open_times = open_times;
  this->unload_time = unload_time;
  this->bias = bias;
  this->load = load;
  this->index = index;
}

Graph::Graph() {}

void Graph::add_node(
  std::string name,
  std::pair<int, int> location,
  std::pair<int, int> times,
  int unload_time,
  double load,
  double bias,
  int index
) {
  m_nodes_[index] = Node(location.first, location.second, times, unload_time, load, bias, index);
  m_node_names.push_back(name);
}

void Graph::add_edge(int a, int b, int distance) { m_dist_[a][b] = distance; }

double Graph::get_distance(int a, int b) const { return m_dist_[a][b]; }

double Graph::get_cost(int a, int b) const { return m_dist_[a][b] - m_nodes_[b].bias; }

int Graph::get_node_unpack_time(int name) const { return m_nodes_[name].unload_time; }
int Graph::get_num_nodes() const { return m_node_names.size(); };

std::pair<int, int> Graph::get_node_times(int name) const { return m_nodes_[name].open_times; }

double Graph::get_node_load(int name) const { return m_nodes_[name].load; }

Node Graph::get_node_data(int name) const {
  return m_nodes_[name];
  // assert(m_nodes.find(name) != m_nodes.end());
  // return m_nodes.find(name)->second;
};
std::vector<std::string> Graph::get_node_names() const { return m_node_names; }