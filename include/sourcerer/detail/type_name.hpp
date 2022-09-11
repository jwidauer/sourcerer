#pragma once

#include <sourcerer/detail/concepts.hpp>
#include <sourcerer/detail/node_forwards.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace sourcerer::detail {

// Implements a constexpr type to human readable string conversion.
// Taken from: https://stackoverflow.com/a/59522794/13170756

namespace impl {

template <typename T>
[[nodiscard]] constexpr std::string_view raw_type_name() {
#ifndef _MSC_VER
  return __PRETTY_FUNCTION__;
#else
  return __FUNCSIG__;
#endif
}

struct TypeNameFormat {
  std::size_t junk_leading = 0;
  std::size_t junk_total = 0;
};

constexpr TypeNameFormat type_name_format = [] {
  TypeNameFormat ret;
  std::string_view sample = raw_type_name<int>();
  ret.junk_leading = sample.find("int");
  ret.junk_total = sample.size() - 3;
  return ret;
}();
static_assert(type_name_format.junk_leading != std::size_t(-1),
              "Unable to determine the type name format on this compiler.");

template <typename T>
static constexpr auto type_name_storage = [] {
  std::array<char, raw_type_name<T>().size() - type_name_format.junk_total + 1> ret{};
  std::copy_n(raw_type_name<T>().data() + type_name_format.junk_leading, ret.size() - 1,
              ret.data());
  return ret;
}();

}  // namespace impl

template <typename T>
[[nodiscard]] constexpr std::string type_name() {
  return {impl::type_name_storage<T>.data(), impl::type_name_storage<T>.size() - 1};
}

template <typename T>
[[nodiscard]] constexpr const char* type_name_cstr() {
  return impl::type_name_storage<T>.data();
}

inline std::string type_name(const children_t& children) noexcept {
  return std::visit(detail::overloaded{[](const null_t&) { return "null"; },
                                       [](const value_t&) { return "value"; },
                                       [](const array_t&) { return "array"; },
                                       [](const object_t&) { return "object"; }},
                    children);
}

template <child T>
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
    static_assert(detail::always_false_v<T>, "Unknown type");
  }

  return "";  // Make compiler happy.
}

}  // namespace sourcerer::detail
