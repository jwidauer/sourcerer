#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace sourcerer {

class Node;

namespace detail {

using index_t = size_t;
using key_t = std::string;

using null_t = std::monostate;
using value_t = std::string;
using array_t = std::vector<std::unique_ptr<Node>>;
using object_t = std::map<key_t, std::unique_ptr<Node>, std::less<>>;

using children_t = std::variant<detail::null_t, detail::value_t, detail::array_t, detail::object_t>;

// Helper for static asserts.
template <class>
inline constexpr bool always_false_v = false;

// Helper type for visitors
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

}  // namespace detail

}  // namespace sourcerer