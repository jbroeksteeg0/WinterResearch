#include "DynamicBitset.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

DynamicBitset::DynamicBitset(size_t size) : m_size(size) {
  // The min number of ints to store all `size` bits
  assert(size <= 128);
  memset(m_elems, 0, 16);
}

bool DynamicBitset::get(size_t bit) const {
  assert(bit < (size_t)m_size);

  // The int the bit is stored in
  int int_value = m_elems[bit / 8];

  // Return its value
  return int_value & (1 << (bit % 8));
}

bool DynamicBitset::is_subset_of(const DynamicBitset &other) const {
  assert(m_size == other.m_size);

  for (size_t i = 0; i < (m_size + 7) / 8; i++) {
    if ((m_elems[i] & other.m_elems[i]) != m_elems[i]) {
      return false;
    }
  }

  return true;
}

bool DynamicBitset::is_superset_of(const DynamicBitset &other) const {
  return other.is_subset_of(*this);
}

void DynamicBitset::set(size_t bit, bool value) {
  assert(bit < (size_t)m_size);

  // The ind of the int the bit is stored in
  int int_ind = bit / 8;

  int mask = 1 << (bit % 8);

  // Setting it to true
  if (value) {
    m_elems[int_ind] |= mask;
  } else {    // Setting it to false
    m_elems[int_ind] &= ~mask;
  }
}

size_t DynamicBitset::get_size() const { return m_size; }

std::string DynamicBitset::to_string() const {
  std::string ret = "";
  for (size_t i = 0; i < m_size; i++) {
    int8_t num = m_elems[i / 8];

    ret += std::to_string((num & (1 << (i % 8))) != 0);
  }

  return ret;
}