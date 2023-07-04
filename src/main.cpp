#include "DynamicBitset.h"
#include "Graph.h"
#include "NDTree.h"
#include "State.h"
#include <algorithm>
#include <cassert>
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
#include <unordered_map>

/*
  dp[node][load][time][bitmask] = cost

  n <= 100
  load <= 200
  time <= 1200


*/
int vehicle_capacity;
Graph graph;

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

  std::vector<std::string> location_names;
  std::map<std::string, std::pair<int, int>> node_location, node_times;
  std::map<std::string, int> node_unload_time, node_load, node_index;
  std::map<std::string, float> node_bias;
  std::map<int, std::string> node_name;

  int ind = 0;
  while (std::getline(file, curr_line)) {
    std::string dest_name;
    int x, y;
    int load;
    int time_start, time_end;
    int time_unload;

    ss = std::stringstream(curr_line);
    ss >> dest_name >> x >> y >> load >> time_start >> time_end >> time_unload;

    node_location[dest_name] = {x, y};
    node_times[dest_name] = {time_start, time_end};
    node_unload_time[dest_name] = time_unload;
    node_load[dest_name] = load;
    node_index[dest_name] = ind;
    node_name[ind++] = dest_name;

    location_names.push_back(dest_name);
  }

  // Parse the biases
  if (!std::filesystem::exists(biases_filename)) {
    std::cout << "ERROR: file '" << biases_filename << "' does not exist" << std::endl;
    exit(1);
  }
  std::ifstream bias_file = std::ifstream(biases_filename);

  for (int i = 1; i < location_names.size(); i++) {
    float bias;
    bias_file >> bias;

    node_bias[node_name[i]] = bias;
  }

  // Add in all the nodes
  for (const auto &node_name : location_names) {
    graph.add_node(
      node_name,
      node_location[node_name],
      node_times[node_name],
      node_unload_time[node_name],
      node_load[node_name],
      node_bias[node_name],
      node_index[node_name]
    );
  }

  // Add all possible edges into the graph
  for (auto location1 = location_names.begin(); location1 != location_names.end(); location1++) {
    for (auto location2 = location_names.begin(); location2 != location_names.end(); location2++) {
      if (location1 != location2) {
        double dx = node_location[*location1].first - node_location[*location2].first;
        double dy = node_location[*location1].second - node_location[*location2].second;

        double dist = std::sqrt(dx * dx + dy * dy);

        graph.add_edge(*location1, *location2, ceil(dist));
      }
    }
  }
}

auto node_cmp = [](State a, State b) { return a.time < b.time; };

std::map<std::string, NDTree<State, 3>> node_states;
void shortest_paths() {

  int n = graph.get_num_nodes();
  std::vector<std::string> node_names = graph.get_node_names();

  //                          name, time, load, num nodes, cost
  State initial_state = State("R-D", 0, 0.0, n, 0.0);

  std::queue<State> q;
  q.push(initial_state);

  for (const std::string &name : graph.get_node_names()) {
    node_states.insert(
      {name,
       NDTree<State, 3>({
         std::make_pair(0, 1500),     // time
         std::make_pair(0, 200),      // load
         std::make_pair(-5e4, 5e4)    // cost TODO change
       })}
    );
  }

  // Add the initial state
  node_states.find("R-D")->second.add(
    {(double)initial_state.time, initial_state.load, initial_state.cost}, initial_state
  );

  double ans = 0.0;

  int iterations = 0;

  while (q.size()) {
    State curr_state = q.front();
    q.pop();
    // std::cout << curr_state.to_string() << std::endl;
    iterations++;
    if (iterations % 100000 == 0)
      std::cout << iterations << " iterations" << std::endl;

    if (curr_state.node == "R-D")
      ans = std::min(ans, curr_state.cost);

    const std::string from = curr_state.node;
    for (const auto &to : node_names) {
      if (from == to)
        continue;

      Node to_node = graph.get_node_data(to);

      // Cannot visit a node it's already been to
      if (curr_state.has_been_to(to_node.index))
        continue;

      int new_time =
        curr_state.time + graph.get_node_unpack_time(from) + graph.get_distance(from, to);

      if (new_time < to_node.open_times.first) {
        new_time = to_node.open_times.first;
      }

      // Cannot visit a node if it has too much load
      if (curr_state.load + to_node.load > vehicle_capacity)
        continue;

      // Cannot visit a node if it doesn't arrive in time
      if (new_time > to_node.open_times.second)
        continue;

      DynamicBitset new_seen = curr_state.nodes_seen;
      new_seen.set(to_node.index, true);

      State new_state = State(
        to,                                           // position
        new_time,                                     // time
        curr_state.load + to_node.load,               // load
        new_seen,                                     // nodes seen
        curr_state.cost + graph.get_cost(from, to)    // cost
      );
      bool add_state = true;
      std::vector<State> possible_better_states;
      node_states.find(to)->second.query_prefix(
        {(double)new_state.time, new_state.load, new_state.cost}, possible_better_states
      );

      for (const auto &check_state : possible_better_states) {
        if (check_state.nodes_seen.is_subset_of(new_state.nodes_seen)) {
          add_state = false;
          break;
        }
      }

      // std::cout << "Checking " << possible_better_states.size() << " out of "
      //           << node_states.find(to)->second.size() << ": " << add_state << std::endl;

      if (add_state) {
        // Add the new state
        node_states.find(to)->second.add(
          {(double)new_state.time, new_state.load, new_state.cost}, new_state
        );
        q.push(new_state);
      }
    }
  }

  std::cout << "Shortest path is " << ans << std::endl;
}

int main(int argc, char **argv) {

  NDTree<int, 2> tree({std::make_pair(0, 100), std::make_pair(0, 100)});
  tree.add({5.0, 5.0}, 2);
  tree.add({5.0, 5.0}, 3);

  std::cout << "before delete" << std::endl;
  tree.delete_point({5.0, 5.0}, 3);
  tree.delete_point({5.0, 5.0}, 2);
  std::cout << "after delete" << std::endl;
  std::vector<int> ans;
  tree.query_prefix({10.0, 10.0}, ans);

  for (int i : ans)
    std::cout << i << " ";
  std::cout << std::endl;

  return 1;
  if (argc < 3) {
    std::cout << "Run with filename and number of iterations" << std::endl;
    return 1;
  }

  // std::cout << "What file do you want to open? (e.g. c101_75)" << std::endl;
  std::string file_name;
  file_name = std::string(argv[1]);
  // std::cin >> file_name;

  // std::cout << "How many iterations? (e.g. 20)" << std::endl;
  std::string iterations_string = std::string(argv[2]);
  int iterations = std::stoi(iterations_string);
  // std::cin >> iterations;

  // Don't print with scientific notation
  std::cout << std::fixed;

  populate_graph(file_name, iterations);
  shortest_paths();
  return 0;
}
