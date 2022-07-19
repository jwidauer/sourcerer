#pragma once

#include <sourcerer/detail/types.hpp>

#include <charconv>
#include <type_traits>

namespace sourcerer {

namespace detail {

// template <class T, typename std::enable_if_t<std::is_same_v<T, null_t>, bool> = true>
template <class T>
T convert(const null_t& /*node*/) {
  return {};
}

template <class T>
T convert(const array_t& /*node*/) {
  return {};
}

template <class T>
T convert(const object_t& /*node*/) {
  return {};
}

template <class T, typename std::enable_if<std::is_same_v<T, value_t>, bool>::type = true>
T convert(const value_t& node) {
  return node;
}

template <class T, typename std::enable_if<std::is_same_v<T, bool>, bool>::type = true>
T convert(const value_t& node) {
  return node == "true" || node == "1";
}

template <class T, typename std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>,
                                             bool> = true>
// std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, int>::type = 0>
T convert(const value_t& node) {
  std::string_view sv{node};
  T value;
  auto [ptr, ec]{std::from_chars(sv.begin(), sv.end(), value)};
  if (ec != std::errc{}) {
    throw std::runtime_error{"Failed to convert value to integral type"};
  }

  return value;
}

// template <class T, typename std::enable_if<std::is_floating_point_v<T>, int>::type = 0>
// T convert(const value_t& node) {
//   T value;
//   auto [ptr, ec]{std::from_chars(node.begin(), node.end(), value)};
//   if (ec != std::errc{}) {
//     throw std::runtime_error{"Failed to convert value to floating point type"};
//   }

//   return value;
// }

}  // namespace detail

}  // namespace sourcerer
