#pragma once
#include <iostream>
#include <vector>

class State;

class DynamicBitset {
public:
  DynamicBitset(size_t size);

  void set(size_t bit, bool value);

  bool get(size_t bit) const;
  bool is_subset_of(const DynamicBitset &other) const;
  bool is_superset_of(const DynamicBitset &other) const;
  size_t get_size() const;
  std::string to_string() const;

  __int128 m_elems;

private:
  size_t m_size;

  friend class State;
};