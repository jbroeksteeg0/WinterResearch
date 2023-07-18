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