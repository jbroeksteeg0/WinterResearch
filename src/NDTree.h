#pragma once
#include <array>
#include <cassert>
#include <compare>
#include <iostream>
#include <map>
#include <optional>
#include <stdint.h>
#include <unordered_map>
#include <utility>
#include <vector>

#define EPS ((double)1e-20)

template <typename T, int N> class NDTree {
public:
  NDTree(std::array<std::pair<double, double>, N> bounds) {
    m_bounds = bounds;
    m_count = 0;

    for (int i = 0; i < m_nodes.size(); i++) {
      m_nodes[i] = nullptr;
    }

    m_iter_count = new int(0);
    m_value_map = new std::unordered_map<int, std::pair<std::array<double, N>, std::vector<T>>>();
  }
  bool is_subdivided() const { return m_nodes[0] != nullptr; }
  size_t size() const { return m_count; }

  // The mask of the child who contains coords
  int get_child_mask(std::array<double, N> coords) const {
    assert(is_subdivided());
    int mask = 0;
    for (int i = 0; i < N; i++) {
      if (coords[i] > ((m_bounds[i].first + m_bounds[i].second) / 2))
        mask |= (1 << i);
    }
    return mask;
  }
  void subdivide_self() {
    assert(!is_subdivided());
    for (int mask = 0; mask < (1 << N); mask++) {
      std::array<std::pair<double, double>, N> new_bounds;

      for (int bit = 0; bit < N; bit++) {
        if (mask & (1 << bit)) {    // positive direction
          new_bounds[bit] = std::make_pair(
            (m_bounds[bit].first + m_bounds[bit].second) / 2 + EPS, m_bounds[bit].second
          );
        } else {    // negative direction
          new_bounds[bit] =
            std::make_pair(m_bounds[bit].first, (m_bounds[bit].first + m_bounds[bit].second) / 2);
        }
      }

      m_nodes[mask] = new NDTree<T, N>(new_bounds, m_iter_count, m_value_map);
    }
  }

  void add(std::array<double, N> coords, T value) {
    for (int i = 0; i < N; i++) {
      assert(coords[i] >= m_bounds[i].first && coords[i] <= m_bounds[i].second);
    }

    // EMPTY LEAF, CREATE A NEW VEC
    if (m_count == 0 && !is_subdivided()) {
      m_value = ++(*m_iter_count);
      (*m_value_map)[*m_value].first = coords;
      (*m_value_map)[*m_value].second.push_back(value);
    } else if (m_count > 0 && !is_subdivided()) {
      // LEAF WITH VALUES, POSSIBLY SPLIT
      assert(m_value.has_value());
      auto leaf_coords = (*m_value_map)[*m_value].first;
      // for (auto d : leaf_coords)
      //   std::cout << d << " ";
      // std::cout << std::endl;
      // for (auto d : coords)
      //   std::cout << d << " ";
      // std::cout << std::endl;
      if (coords == leaf_coords) {
        // DON'T NEED TO SPLIT
        (*m_value_map)[*m_value].second.push_back(value);
      } else {
        // DIFFERENT COORDS, NEED TO SPLIT
        subdivide_self();

        // If an old value exists, get rid of it
        if (m_value.has_value()) {
          // Transfer the old in
          // if it has a value we're fucked
          assert(!m_nodes[get_child_mask(leaf_coords)]->m_value.has_value());
          m_nodes[get_child_mask(leaf_coords)]->set_ind(*m_value);
          m_value = std::nullopt;
        }

        m_nodes[get_child_mask(coords)]->add(coords, value);
      }
    } else {
      // NON-LEAF (probably with values)
      m_nodes[get_child_mask(coords)]->add(coords, value);
    }

    m_count++;
  }

  void query_prefix(std::array<double, N> bounds, std::vector<T> &output) {
    for (int i = 0; i < N; i++) {
      assert(bounds[i] >= m_bounds[i].first /* && bounds[i] <= m_bounds[i].second*/);
    }

    // Empty
    if (m_count == 0) {
      return;
    }

    if (!is_subdivided()) {

      if (m_value.has_value()) {
        bool in_bounds = true;
        for (int i = 0; i < N; i++) {
          in_bounds &= (*m_value_map)[*m_value].first[i] <= bounds[i];
        }

        if (in_bounds && m_value.has_value()) {
          for (auto v : (*m_value_map)[*m_value].second)
            output.push_back(v);
        }
      }
      return;
    }

    // recurse
    for (int mask = 0; mask < (1 << N); mask++) {
      bool in_new_bounds = true;
      for (int bit = 0; bit < N; bit++) {
        std::pair<double, double> new_bounds = m_nodes[mask]->m_bounds[bit];

        in_new_bounds &= bounds[bit] >= new_bounds.first;
      }
      if (in_new_bounds) {
        // std::cout << "Looking for coords " << bounds[0] << "," << bounds[1] << std::endl;
        // std::cout << "Mask " << mask << " is good" << std::endl;
        m_nodes[mask]->query_prefix(bounds, output);
      }
    }
  }

  void delete_point(std::array<double, N> coords, T value) {
    if (!is_subdivided()) {
      assert(m_value.has_value());

      // auto iter = std::find(
      //   (*m_value_map)[*m_value].second.begin(), (*m_value_map)[*m_value].second.end(), value
      // );
      // assert(iter != (*m_value_map)[*m_value].second.end());

      auto iter = (*m_value_map)[*m_value].second.begin();
      while (iter != (*m_value_map)[*m_value].second.end()) {
        if (*iter == value) {
          (*m_value_map)[*m_value].second.erase(iter);
          break;
        }
        iter++;
      }

      // (*m_value_map)[*m_value].second.erase(value);

      if ((*m_value_map)[*m_value].second.empty())
        m_value = std::nullopt;
      m_count--;
      return;
    }

    // TODO optimise
    for (int mask = 0; mask < (1 << N); mask++) {
      std::array<std::pair<double, double>, N> new_bounds;

      bool in_new_bounds = true;
      for (int bit = 0; bit < N; bit++) {
        std::array<std::pair<double, double>, N> new_bounds = m_nodes[mask]->m_bounds;
        assert(m_nodes[mask] != nullptr);

        in_new_bounds &=
          (new_bounds[bit].first <= coords[bit] && new_bounds[bit].second >= coords[bit]);
      }

      if (in_new_bounds) {
        m_nodes[mask]->delete_point(coords, value);
      }
    }
    m_count--;
  }

private:
  std::array<NDTree<T, N> *, 1 << N> m_nodes;
  std::array<std::pair<double, double>, N> m_bounds;
  std::optional<int> m_value;
  int m_count;
  int *m_iter_count = nullptr;

  std::unordered_map<int, std::pair<std::array<double, N>, std::vector<T>>> *m_value_map;

private:
  NDTree(
    std::array<std::pair<double, double>, N> bounds,
    int *iter_count,
    std::unordered_map<int, std::pair<std::array<double, N>, std::vector<T>>> *value_map
  ) {
    m_bounds = bounds;
    m_count = 0;

    for (int i = 0; i < m_nodes.size(); i++) {
      m_nodes[i] = nullptr;
    }
    m_iter_count = iter_count;
    m_value_map = value_map;
  }

  void set_ind(int ind) {
    m_value = ind;
    m_count++;
  }
};
