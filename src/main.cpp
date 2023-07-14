#include "Graph.h"
#include "State.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <map>
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

  // std::vector<std::string> location_names;
  // std::map<std::string, std::pair<int, int>> node_location, node_times;
  // std::map<std::string, int> node_unload_time, node_load, node_index;
  // std::map<std::string, float> node_bias;
  // std::map<int, std::string> node_name;

  int ind = 0;
  while (std::getline(file, curr_line)) {
    std::string dest_name;
    int x, y;
    int load;
    int time_start, time_end;
    int time_unload;

    ss = std::stringstream(curr_line);
    ss >> dest_name >> x >> y >> load >> time_start >> time_end >> time_unload;

    // node_location[dest_name] = {x, y};
    // node_times[dest_name] = {time_start, time_end};
    // node_unload_time[dest_name] = time_unload;
    // node_load[dest_name] = load;
    // node_index[dest_name] = ind;
    // node_name[ind++] = dest_name;

    nodes[ind] = Node(x, y, {time_start, time_end}, time_unload, load, 0, ind);
    ind++;

    // location_names.push_back(dest_name);
  }
  num_nodes = ind;

  // Parse the biases
  if (!std::filesystem::exists(biases_filename)) {
    std::cout << "ERROR: file '" << biases_filename << "' does not exist" << std::endl;
    exit(1);
  }
  std::ifstream bias_file = std::ifstream(biases_filename);

  for (size_t i = 1; i < num_nodes; i++) {
    float bias;
    bias_file >> bias;

    // node_bias[node_name[i]] = bias;
    nodes[i].bias = bias;
  }

  // Add in all the nodes
  // for (const auto &node_name : location_names) {
  //   graph.add_node(
  //     node_name,
  //     node_location[node_name],
  //     node_times[node_name],
  //     node_unload_time[node_name],
  //     node_load[node_name],
  //     node_bias[node_name],
  //     node_index[node_name]
  //   );
  // }

  for (size_t i = 0; i < num_nodes; i++) {
    for (size_t j = 0; j < num_nodes; j++) {
      if (i != j) {
        double dx = nodes[i].x - nodes[j].x;
        double dy = nodes[i].y - nodes[j].y;

        double node_dist = std::sqrt(dx * dx + dy * dy);

        // graph.add_edge(i, j, ceil(dist));
        dist[i][j] = ceil(node_dist);
      }
    }
  }
}

double inline get_cost(int a, int b) { return dist[a][b] - nodes[b].bias; }

template <typename IntType> void shortest_paths() {
  // ------------------------ Initialise an ND-Tree for each node
  int n = num_nodes;
  State<IntType> initial_state = State(0, 0, 0.0, (IntType)0, 0.0);
  std::vector<State<IntType>> q;
  int q_pointer = 0;
  q.push_back(initial_state);
  std::vector<std::vector<State<IntType>>> prev_states(n);
  // ========================== Push the initial state

  double ans = 0.0;
  int iterations = 0;

  // ========================== Run the BFS
  while (q_pointer < q.size()) {
    State<IntType> curr_state = q[q_pointer++];

    iterations++;
    if (iterations % 10000 == 0)
      std::cout << iterations << " iterations, currently at time " << curr_state.time << std::endl;

    if (curr_state.node == 0)
      ans = std::min(ans, curr_state.cost);

    int from = curr_state.node;

    // ========================== Iterate over every destination
    for (int to = 0; to < n; to++) {
      if (from == to)
        continue;

      Node &to_node = nodes[to];

      // ========================== Exit early if the new state would not be valid
      if (curr_state.has_been_to(to_node.index))
        continue;

      int new_time = curr_state.time + dist[from][to];

      if (new_time < to_node.open_times.first) {
        new_time = to_node.open_times.first;
      }

      // Cannot visit a node if it has too much load
      if (curr_state.load + to_node.load > vehicle_capacity)
        continue;

      // Cannot visit a node if it doesn't arrive in time
      if (new_time > to_node.open_times.second)
        continue;

      IntType new_seen = curr_state.nodes_seen;
      new_seen |= (((IntType)1) << to_node.index);

      State new_state = State(
        to,                                     // position
        new_time + to_node.unload_time,         // time
        curr_state.load + to_node.load,         // load
        new_seen,                               // nodes seen
        curr_state.cost + get_cost(from, to)    // cost
      );

      bool add_state = true;

      // std::cout << prev_states[to].size() << std::endl;
      for (State s : prev_states[to]) {
        if ( (s.nodes_seen & new_seen) == s.nodes_seen && s.time <= new_state.time && s.cost <= new_state.cost && s.load <= new_state.load) {
          add_state = false;
          break;
        }
      }

      if (add_state) {
        // ========================== If this state has not been dominated, add it
        q.push_back(new_state);
        prev_states[to].push_back(new_state);
      }
    }
  }

  std::cout << "Shortest path is " << ans << std::endl;
}

int main(int argc, char **argv) {
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
