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