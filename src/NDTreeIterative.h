#pragma once
#include <array>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

#define NUM_TREE_NODES 80000
#define EPS ((double)1e-20)

template <typename T, int N> class NDTree;

template <typename T, int N> struct NDNode {
public:
  int m_index_in_map = -1;
  std::array<int, 1 << N> m_children;
  std::array<std::pair<double, double>, N> m_bounds;
  bool m_is_leaf = true;

  NDNode() {}

private:
  NDNode(std::array<std::pair<double, double>, N> bounds) { m_bounds = bounds; }

  inline int get_child_mask(std::array<double, N> coords) const noexcept {
    assert(!m_is_leaf);
    int mask = 0;
    for (int i = 0; i < N; i++) {
      if (coords[i] > ((m_bounds[i].first + m_bounds[i].second) / 2))
        mask |= (1 << i);
    }
    return mask;
  }

  friend class NDTree<T, N>;
};

template <typename T, int N> class NDTree {
public:
  NDTree() {    // this should never be used
    std::array<std::pair<double, double>, N> bounds;
    m_nodes.resize(NUM_TREE_NODES);
    m_nodes[m_node_counter++] = NDNode<T, N>(bounds);
  }
  NDTree(std::array<std::pair<double, double>, N> bounds) {
    // Initialise root node
    m_nodes.resize(NUM_TREE_NODES);
    m_nodes[m_node_counter++] = NDNode<T, N>(bounds);
    m_value_map.reserve(1e5);
  }

  void add(std::array<double, N> coordinates, T value) {
    int curr_node = 0;

    // Recurse until a leaf is found
    while (!m_nodes[curr_node].m_is_leaf) {
      // Find child to go to
      int direction_mask = m_nodes[curr_node].get_child_mask(coordinates);

      curr_node = m_nodes[curr_node].m_children[direction_mask];
    }

    int map_index = m_nodes[curr_node].m_index_in_map;
    // If it has a value already, you have to handle
    if (map_index != -1) {
      std::array<double, N> old_node_coords = m_value_map[map_index].first;
      // If the coord is the same, add to vector
      if (m_value_map[map_index].first == coordinates) {
        m_value_map[map_index].second.push_back(value);
      } else {    // Otherwise, need to split and delegate them
        split(curr_node);
        // While splits still need to happen
        while (m_nodes[curr_node].get_child_mask(coordinates)
               == m_nodes[curr_node].get_child_mask(old_node_coords)) {
          int direction = m_nodes[curr_node].get_child_mask(coordinates);

          curr_node = m_nodes[curr_node].m_children[direction];
          split(curr_node);
        }

        // they will be in different directions
        int old_node_dir = m_nodes[curr_node].get_child_mask(old_node_coords);
        int new_node_dir = m_nodes[curr_node].get_child_mask(coordinates);

        int old_node_ind = m_nodes[curr_node].m_children[old_node_dir];
        int new_node_ind = m_nodes[curr_node].m_children[new_node_dir];

        // Set the pointer to the old one
        m_nodes[old_node_ind].m_index_in_map = map_index;

        // Create a pointer to the new one
        m_nodes[new_node_ind].m_index_in_map = m_map_index_counter;
        m_value_map.push_back(std::make_pair(coordinates, std::vector<T>{value}));
        m_map_index_counter++;

        assert(curr_node != old_node_ind && curr_node != new_node_ind);
        m_nodes[curr_node].m_index_in_map = -1;
      }
    } else {
      // Otherwise, just set it
      m_nodes[curr_node].m_index_in_map = m_map_index_counter;
      m_value_map.push_back(std::make_pair(coordinates, std::vector<T>{value}));

      m_map_index_counter++;
    }
  }

  std::vector<T> query_prefix_dfs(std::array<double, N> coords) {
    std::vector<T> ans;
    ans.reserve(20);
    //                    at,  child to look at
    std::array<std::pair<int, int>, 20> dfs;
    dfs[0] = {0, 0};
    int dfs_pointer = 0;

    while (dfs_pointer >= 0) {
      assert(dfs_pointer + 1 <= 20);
      const int curr_node = dfs[dfs_pointer].first;
      // std::cout << "Looking at range: ";

      if (dfs[dfs_pointer].second >= (1 << N) || (m_nodes[curr_node].m_is_leaf && dfs[dfs_pointer].second > 0)) {
        dfs_pointer--;
        if (dfs_pointer >= 0)
          dfs[dfs_pointer].second++;

        continue;
      }

      if (m_nodes[curr_node].m_is_leaf) {
        if (m_nodes[curr_node].m_index_in_map != -1) {
          bool in_bounds = true;
          int ind = m_nodes[curr_node].m_index_in_map;
          for (int i = 0; i < N; i++) {
            in_bounds &= coords[i] >= m_value_map[ind].first[i];
          }

          if (in_bounds) {
            for (auto t : m_value_map[m_nodes[curr_node].m_index_in_map].second) {
              ans.push_back(std::move(t));
            }
          }
        }
        dfs_pointer--;
        if (dfs_pointer >= 0)
          dfs[dfs_pointer].second++;
      } else {    // Iterate over all subtrees
        int new_ind = m_nodes[curr_node].m_children[dfs[dfs_pointer].second];

        bool new_range_contains = true;
        auto new_bounds = m_nodes[new_ind].m_bounds;
        for (int i = 0; i < N; i++) {
          new_range_contains &= coords[i] >= new_bounds[i].first;
        }
        if (new_range_contains)
          dfs[++dfs_pointer] = {new_ind, 0};
        else
          dfs[dfs_pointer].second++;
      }
    }
    return ans;
  }
  std::vector<T> query_prefix(std::array<double, N> coords) {
    std::vector<T> ans;

    std::deque<int> bfs;
    bfs.push_back(0);

    while (bfs.size()) {
      int curr_node = bfs.front();
      bfs.pop_front();

      if (m_nodes[curr_node].m_is_leaf) {    // Push all nodes
        if (m_nodes[curr_node].m_index_in_map != -1) {
          bool in_bounds = true;
          int ind = m_nodes[curr_node].m_index_in_map;
          for (int i = 0; i < N; i++) {
            in_bounds &= coords[i] >= m_value_map[ind].first[i];
          }

          if (!in_bounds)
            continue;
          for (auto t : m_value_map[m_nodes[curr_node].m_index_in_map].second) {
            ans.push_back(t);
          }
        }
      } else {    // Iterate over all subtrees
        assert(!m_nodes[curr_node].m_is_leaf);

        auto children = m_nodes[curr_node].m_children;
        for (int mask = 0; mask < (1 << N); mask++) {
          bool in_new_bounds = true;
          auto new_bounds = m_nodes[children[mask]].m_bounds;
          for (int bit = 0; bit < N; bit++) {
            in_new_bounds &= coords[bit] >= new_bounds[bit].first;
          }
          if (in_new_bounds) {
            bfs.push_back(children[mask]);    // push the new ind
          }
        }
      }
    }
    return std::move(ans);
  }

private:
  // std::array<NDNode<T, N>, NUM_NODES> m_nodes;
  std::vector<NDNode<T, N>> m_nodes;
  std::vector<std::pair<std::array<double, N>, std::vector<T>>> m_value_map;
  int m_node_counter = 0, m_map_index_counter = 0;

private:
  void split(int node) {
    m_nodes[node].m_is_leaf = false;

    for (int mask = 0; mask < (1 << N); mask++) {
      // Calculate the new bounds
      std::array<std::pair<double, double>, N> new_bounds;
      std::array<std::pair<double, double>, N> old_bounds = m_nodes[node].m_bounds;
      for (int bit = 0; bit < N; bit++) {
        if ((mask & (1 << bit)) == 0) {    // negative direction
          new_bounds[bit] = std::make_pair(
            old_bounds[bit].first, (old_bounds[bit].first + old_bounds[bit].second) / 2
          );
        } else {
          new_bounds[bit] = std::make_pair(
            (old_bounds[bit].first + old_bounds[bit].second) / 2 + EPS, old_bounds[bit].second
          );
        }
      }

      // Give it a new child
      m_nodes[m_node_counter] = NDNode<T, N>(new_bounds);
      m_nodes[node].m_children[mask] = m_node_counter;

      m_node_counter++;
    }
    assert(m_node_counter < NUM_TREE_NODES);
  }
};
