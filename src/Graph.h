#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define NUM_NODES 102

struct Node {
  int16_t open_time, close_time;
  int unload_time;
  int load;
  double bias;
  Node(){};
  Node(int16_t open_time, int16_t close_time, int unload_time, int load, double bias);
};