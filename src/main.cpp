#include "Graph.h"
#include "State.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

/*
  dp[node][load][time][bitmask] = cost

  n <= 100
  load <= 200
  time <= 1200


*/
int vehicle_capacity;
int num_nodes;

std::array<Node, 101> nodes;
std::array<std::array<int, 101>, 101> dist;
std::array<std::array<double, 101>, 101> cost;

void populate_graph(std::string data_file, int iteration) {
  std::string data_filename = "Archive/solomon_instances/" + data_file + ".txt";

  // Create the file path to the file containing the biases
  std::string iter_string = "";
  int num_zeros = 4 - ((iteration == 0) ? 0 : (int)log10((double)iteration) + 1);

  for (int i = 0; i < num_zeros; i++)
    iter_string += "0";

  if (iteration != 0)
    iter_string += std::to_string(iteration);

  std::string biases_filename =
    "Archive/dual_values_dev/pricing_" + data_file + "_iter_" + iter_string + ".txt";

  std::cout << "Opening paths:" << std::endl;
  std::cout << "  " << data_filename << std::endl;
  std::cout << "  " << biases_filename << std::endl;

  // Read from the data file
  if (!std::filesystem::exists(data_filename)) {
    std::cout << "ERROR: file '" << data_filename << "' does not exist" << std::endl;
    exit(1);
  }

  std::ifstream file = std::ifstream(data_filename);

  std::string header_line;
  std::getline(file, header_line);

  // Parse the vehicle capacity
  std::string curr_line;
  std::getline(file, curr_line);

  std::string temp;
  std::stringstream ss(curr_line);

  // Read a string and then the capacity
  ss >> temp >> vehicle_capacity;

  // Empty line, then useless line
  std::getline(file, curr_line);
  std::getline(file, curr_line);

  int ind = 0;

  std::pair<int, int> positions[101];
  while (std::getline(file, curr_line)) {
    std::string dest_name;
    int x, y;
    int load;
    int time_start, time_end;
    int time_unload;

    ss = std::stringstream(curr_line);
    ss >> dest_name >> x >> y >> load >> time_start >> time_end >> time_unload;

    nodes[ind] = Node(time_start, time_end, time_unload, load, 0);
    positions[ind] = {x, y};
    ind++;
  }
  num_nodes = ind;

  // Parse the biases
  if (!std::filesystem::exists(biases_filename)) {
    std::cout << "ERROR: file '" << biases_filename << "' does not exist" << std::endl;
    exit(1);
  }
  std::ifstream bias_file = std::ifstream(biases_filename);

  for (size_t i = 1; i < num_nodes; i++) {
    double bias;
    bias_file >> bias;

    nodes[i].bias = bias;
  }

  for (size_t i = 0; i < num_nodes; i++) {
    for (size_t j = 0; j < num_nodes; j++) {
      if (i != j) {
        const double dx = positions[i].first - positions[j].first;
        const double dy = positions[i].second - positions[j].second;

        const double node_dist = std::sqrt(dx * dx + dy * dy);
        assert(node_dist < 16000);
        dist[i][j] = ceil(node_dist);
        cost[i][j] = dist[i][j] - nodes[j].bias;
      }
    }
  }
}

template <typename IntType> void shortest_paths() {

  const int n = num_nodes;
  State<IntType> initial_state(0, 0, 0.0, (IntType)0, 0.0, 0);
  std::vector<std::vector<State<IntType>>> prev_states(n);

  using TI = std::tuple<int, int, int>;
  std::priority_queue<TI, std::vector<TI>, std::greater<TI>> q;
  q.push({0, 0, 0});    // {time, node, index}
  prev_states[0].push_back(initial_state);
  // ========================== Push the initial state

  double ans = 0.0;
  int iterations = 0;

  // ========================== Run the BFS
  while (q.size()) {
    const auto &t = q.top();
    State<IntType> &curr_state = prev_states[std::get<1>(t)][std::get<2>(t)];
    q.pop();

    iterations++;
    if (iterations % 10000 == 0)
      std::cout << iterations << " iterations, currently at time " << curr_state.time << std::endl;

    if (curr_state.node == 0)
      ans = std::min(ans, curr_state.cost);

    int from = curr_state.node;

    // ========================== Iterate over every destination
    for (int to = 0; to < n; to++) {
      Node &to_node = nodes[to];
      assert(to_node.unload_time >= 0);

      // ========================== Exit early if the new state would not be valid
      int possible_new_time = curr_state.time + dist[from][to];
      int new_time = std::max((int)to_node.open_time, possible_new_time);
      IntType new_seen = curr_state.nodes_seen;
      new_seen |= (((IntType)1) << to);

      const bool skip = from == to || new_time > to_node.close_time
                        || curr_state.load + to_node.load > vehicle_capacity
                        || new_seen == curr_state.nodes_seen;

      if (skip)
        continue;

      for (size_t i = 0; i < prev_states[to].size(); i++) {
        const State<IntType> &s = prev_states[to][i];
        assert(s.time <= curr_state.time);
        if ( i != curr_state.index_in_prev && (s.nodes_seen & new_seen) == s.nodes_seen && s.cost <= curr_state.cost+cost[from][to] && s.load <= curr_state.load+to_node.load) {
          goto LOOPEND;
        }

        // Insertion sort
        if (s.cost > curr_state.cost + cost[from][to]) {
          // New scope so that label works
          State new_state = State(
            to,                                  // position
            new_time + to_node.unload_time,      // time
            curr_state.load + to_node.load,      // load
            new_seen,                            // nodes seen
            curr_state.cost + cost[from][to],    // cost
            i
          );

          // ========================== If this state has not been dominated, add it
          q.push({new_state.time, to, i});
          prev_states[to].insert(prev_states[to].begin() + i, new_state);

          goto LOOPEND;
        }
      }

      {
        // Otherwise, insert on the end
        State new_state = State(
          to,                                  // position
          new_time + to_node.unload_time,      // time
          curr_state.load + to_node.load,      // load
          new_seen,                            // nodes seen
          curr_state.cost + cost[from][to],    // cost
          prev_states[to].size()
        );
        assert(
          prev_states[to].empty() || curr_state.cost + cost[from][to] >= prev_states[to].back().cost
        );

        // ========================== If this state has not been dominated, add it
        q.push({new_state.time, new_state.node, prev_states[to].size()});
        prev_states[to].push_back(new_state);
      }
    LOOPEND:;
    }
  }

  std::cout << "Shortest path is " << ans << std::endl;
}

int main(int argc, char **argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);

  if (argc < 3) {
    std::cout << "Run with filename and number of iterations, e.g. './main c101_25 56'"
              << std::endl;
    return 1;
  }

  std::string file_name;
  file_name = std::string(argv[1]);

  std::string iterations_string = std::string(argv[2]);
  int iterations = std::stoi(iterations_string);

  // Don't print with scientific notation
  std::cout << std::fixed;

  populate_graph(file_name, iterations);

  if (num_nodes <= 32) {
    shortest_paths<int32_t>();
  } else if (num_nodes <= 64) {
    shortest_paths<int64_t>();
  } else {
    shortest_paths<__int128>();
  }
  return 0;
}
