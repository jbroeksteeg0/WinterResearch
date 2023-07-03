#pragma once
#include <array>
#include <cassert>
#include <compare>
#include <iostream>
#include <optional>
#include <stdint.h>
#include <utility>
#include <vector>

template <typename T, int N> class NDTree {
public:
  NDTree(std::array<std::pair<double, double>, N> bounds) {
    m_bounds = bounds;
    m_count = 0;

    for (int i = 0; i < m_nodes.size(); i++) {
      m_nodes[i] = nullptr;
    }
  }
  bool is_subdivided() const { return m_nodes[0] != nullptr; }
  void add(std::array<double, N> coords, T value) {
    if (m_count == 0 && !is_subdivided()) {
      m_value = {value, coords};
    } else {    // subdivide

      std::optional<std::pair<T, std::array<double, N>>> old_value = m_value;

      // Subdivide if not already done
      if (!is_subdivided()) {
        if (m_value.has_value()) {
          m_value = std::nullopt;
        }
        for (int mask = 0; mask < (1 << N); mask++) {
          std::array<std::pair<double, double>, N> new_bounds;

          for (int bit = 0; bit < N; bit++) {
            if (mask & (1 << bit)) {    // positive direction
              new_bounds[bit] = std::make_pair(
                (m_bounds[bit].first + m_bounds[bit].second) / 2, m_bounds[bit].second
              );
            } else {    // negative direction
              new_bounds[bit] = std::make_pair(
                m_bounds[bit].first, (m_bounds[bit].first + m_bounds[bit].second) / 2
              );
            }
          }

          m_nodes[mask] = new NDTree<T, N>(new_bounds);
          assert(!m_nodes[mask]->is_subdivided());
        }
      }
      for (int mask = 0; mask < (1 << N); mask++) {
        std::array<std::pair<double, double>, N> new_bounds;

        bool curr_in_new_bounds = true, old_in_new_bounds = true;
        for (int bit = 0; bit < N; bit++) {
          assert(m_nodes[mask] != nullptr);
          new_bounds[bit] = m_nodes[mask]->m_bounds[bit];

          curr_in_new_bounds &=
            (new_bounds[bit].first <= coords[bit] && new_bounds[bit].second > coords[bit]);

          old_in_new_bounds &=
            (old_value.has_value() && new_bounds[bit].first <= old_value->second[bit]
             && new_bounds[bit].second > old_value->second[bit]);
        }

        if (curr_in_new_bounds) {
          m_nodes[mask]->add(coords, value);
        }
        if (old_in_new_bounds) {
          m_nodes[mask]->add(old_value->second, old_value->first);
        }
      }

      // find child with new value in it
    }
    m_count++;
  }

  void query_prefix(std::array<double, N> bounds, std::vector<T> &output) {
    // Empty
    if (m_count == 0) {
      return;
    }

    if (!is_subdivided()) {
      bool in_bounds = true;
      for (int i = 0; i < N; i++) {
        in_bounds &= m_value->second[i] < bounds[i];
      }

      if (in_bounds && m_value.has_value()) {
        output.push_back(m_value->first);
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
        m_nodes[mask]->query_prefix(bounds, output);
      }
    }
  }

  void delete_point(std::array<double, N> coords) {
    if (!is_subdivided()) {
      assert(m_value.has_value());
      if (m_value->second == coords) {
        m_count--;
        m_value = std::nullopt;
      }
      return;
    }

    for (int mask = 0; mask < (1 << N); mask++) {
      std::array<std::pair<double, double>, N> new_bounds;

      bool in_new_bounds = true;
      for (int bit = 0; bit < N; bit++) {
        std::array<std::pair<double, double>, N> new_bounds = m_nodes[mask]->m_bounds;
        assert(m_nodes[mask] != nullptr);

        in_new_bounds &=
          (new_bounds[bit].first <= coords[bit] && new_bounds[bit].second > coords[bit]);
      }

      if (in_new_bounds) {
        m_nodes[mask]->delete_point(coords);
      }
    }
    m_count--;
  }

private:
  std::array<NDTree<T, N> *, 1 << N> m_nodes;
  std::array<std::pair<double, double>, N> m_bounds;
  std::optional<std::pair<T, std::array<double, N>>> m_value;
  int m_count;
};
