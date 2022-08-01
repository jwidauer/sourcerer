#pragma once

#include <sourcerer/detail/helpers.hpp>

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sourcerer {

class Node;

namespace detail {

using size_type = std::size_t;

using index_t = size_t;
using key_t = std::string;

using null_t = std::monostate;
using value_t = std::string;
using array_t = std::vector<std::unique_ptr<Node>>;
using object_t = std::unordered_map<key_t, std::unique_ptr<Node>>;

template <class T>
constexpr std::string type_name() noexcept {
  if constexpr (std::is_same_v<T, null_t>) {
    return "null";
  } else if constexpr (std::is_same_v<T, value_t>) {
    return "value";
  } else if constexpr (std::is_same_v<T, array_t>) {
    return "array";
  } else if constexpr (std::is_same_v<T, object_t>) {
    return "object";
  } else {
    static_assert(always_false_v<T>, "Unknown type");
  }

  return "";  // Make compiler happy.
}

}  // namespace detail

}  // namespace sourcerer
