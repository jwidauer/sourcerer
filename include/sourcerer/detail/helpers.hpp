#pragma once

#include <sourcerer/detail/concepts.hpp>
#include <sourcerer/detail/node_forwards.hpp>
#include <sourcerer/detail/type_name.hpp>

#include <iomanip>

namespace sourcerer::detail {

template <child T>
T& try_get(children_t& variant) {
  if (!std::holds_alternative<T>(variant)) {
    throw std::invalid_argument("Node is not of the requested type: " + type_name(variant));
  }
  return std::get<T>(variant);
}

template <child T>
const T& try_get(const children_t& variant) {
  if (!std::holds_alternative<T>(variant)) {
    throw std::invalid_argument("Node is not of the requested type: " + type_name(variant));
  }
  return std::get<T>(variant);
}

}  // namespace sourcerer::detail
