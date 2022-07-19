#pragma once

#include <sourcerer/detail/types.hpp>

#include <string>

namespace sourcerer {

namespace detail {

std::string convert_s(const null_t& /* value */) { return "null"; }

std::string convert_s(const value_t& value) { return value; }

std::string convert_s(const array_t& value) {
  std::string result = "[";
  for (const auto& element : value) {
    result += convert_s(*element);
    result += ", ";
  }
  if (!value.empty()) {
    result.pop_back();
    result.pop_back();
  }
  result += "]";
  return result;
}

std::string convert_s(const object_t& value) {
  std::string result = "{";
  for (const auto& element : value) {
    result += element.first + ": " + convert_s(*element.second) + ", ";
  }
  if (!value.empty()) {
    result.pop_back();
    result.pop_back();
  }
  result += "}";
  return result;
}

std::string convert_s(const Node& node) {
  if (node.is_null()) {
    return convert_s(node.as<null_t>());
  } else if (node.is_value()) {
    return convert_s(node.as<value_t>());
  } else if (node.is_array()) {
    return convert_s(node.as<array_t>());
  } else if (node.is_object()) {
    return convert_s(node.as<object_t>());
  } else {
    return "";
  }
}

}  // namespace detail

}  // namespace sourcerer
