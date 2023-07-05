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
  // Node shouldn't already exist
  assert(m_nodes.find(name) == m_nodes.end());

  // std::cout << "Adding node " << name << ": at (" << location.first << ", " << location.second
  //           << ") with times [" << times.first << "," << times.second << "] unload time "
  //           << unload_time << " and bias " << bias << std::endl;
  m_nodes[name] = Node(location.first, location.second, times, unload_time, load, bias, index);
  m_nodes_[index] = Node(location.first, location.second, times, unload_time, load, bias, index);
  m_node_names.push_back(name);
}

void Graph::add_edge(int a, int b, int distance) {
  // assert(m_nodes.find(a) != m_nodes.end());
  // assert(m_nodes.find(b) != m_nodes.end());

  // std::cout << "Adding edge from " << a << " to " << b << " with dist " << distance << std::endl;
  // m_dist[{a, b}] = distance;
  m_dist_[a][b] = distance;
}

double Graph::get_distance(int a, int b) const {
  // assert(m_dist.find({a, b}) != m_dist.end());

  // return m_dist.find({a, b})->second;
  return m_dist_[a][b];
}

double Graph::get_cost(int a, int b) const {
  // assert(m_dist.find({a, b}) != m_dist.end());

  // return m_dist.find({a, b})->second - m_nodes.find(a)->second.bias;

  // NOTE: applying bias when arriving at node
  // return m_dist.find({a, b})->second - m_nodes.find(b)->second.bias;
  return m_dist_[a][b] - m_nodes_[b].bias;
}

// bool Graph::node_exists(const std::string &name) const {
//   return m_nodes.find(name) != m_nodes.end();
// }

int Graph::get_node_unpack_time(int name) const {
  // assert(m_nodes.find(name) != m_nodes.end());
  // return m_nodes.find(name)->second.unload_time;
  return m_nodes_[name].unload_time;
}
int Graph::get_num_nodes() const { return m_node_names.size(); };

// std::vector<std::string> Graph::get_node_names() const {
//   std::vector<std::string> ret;

//   // map (key,value) -> value to m_nodes
//   std::transform(m_nodes.begin(), m_nodes.end(), std::back_inserter(ret), [](auto p) {
//     return p.first;
//   });

//   return ret;
// }

// int Graph::get_node_ind(const std::string &name) const {
//   assert(m_nodes.find(name) != m_nodes.end());

//   return m_nodes.find(name)->second.index;
// }

std::pair<int, int> Graph::get_node_times(int name) const {
  // assert(m_nodes.find(name) != m_nodes.end());
  // return m_nodes.find(name)->second.open_times;
  return m_nodes_[name].open_times;
}

double Graph::get_node_load(int name) const {
  // assert(m_nodes.find(name) != m_nodes.end());

  return m_nodes_[name].load;
}

Node Graph::get_node_data(int name) const {
  return m_nodes_[name];
  // assert(m_nodes.find(name) != m_nodes.end());
  // return m_nodes.find(name)->second;
};
std::vector<std::string> Graph::get_node_names() const { return m_node_names; }