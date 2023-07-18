#include "Graph.h"
#include <algorithm>
#include <cassert>

Node::Node(int16_t open_time, int16_t close_time, int unload_time, int load, double bias) {
  this->open_time = open_time;
  this->close_time = close_time;
  this->unload_time = unload_time;
  this->bias = bias;
  this->load = load;
}