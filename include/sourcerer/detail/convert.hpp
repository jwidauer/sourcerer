#pragma once

#include <sourcerer/detail/types.hpp>

#include <type_traits>

namespace sourcerer {

class Node;

namespace detail {

template <class T, typename std::enable_if<std::is_same_v<T, null_t>, int>::type = 0>
T convert(const null_t& node) {
  return node;
}

template <class T>
T convert(const array_t& node) {}

template <class T>
T convert(const object_t& node) {}

template <class T, typename std::enable_if<std::is_same_v<T, value_t>, int>::type = 0>
T convert(const value_t& node) {
  return node;
}

template <class T, typename std::enable_if<std::is_same_v<T, bool>, int>::type = 0>
T convert(const value_t& node) {
  return node == "true";
}

}  // namespace detail

}  // namespace sourcerer
