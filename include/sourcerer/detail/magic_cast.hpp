#pragma once

#include <sourcerer/detail/concepts.hpp>
#include <sourcerer/detail/node_forwards.hpp>
#include <sourcerer/detail/type_name.hpp>

#include <charconv>
#include <limits>
#include <type_traits>

namespace sourcerer::detail {

template <class T, class U>
T magic_cast(const U& /*value*/) {
  throw std::invalid_argument{"Can't convert magic_cast " + type_name<U>() + " to " +
                              type_name<T>()};
}

template <class T>
  requires std::constructible_from<T, value_t>
constexpr T magic_cast(const value_t& value) {
  return value;
}

template <class T>
  requires std::constructible_from<value_t, T>
constexpr value_t magic_cast(const T& value) {
  return value;
}

template <>
bool magic_cast(const value_t& value) {
  return value == "true" || value == "1";
}

template <>
value_t magic_cast(const bool& value) {
  return value ? "true" : "false";
}

template <number T>
T magic_cast(const value_t& value) {
  const std::string_view sv{value};
  T result;
  auto [ptr, ec]{std::from_chars(sv.begin(), sv.end(), result)};
  if (ec != std::errc{}) {
    throw std::invalid_argument{"Failed to convert value to type \"" + type_name<T>() +
                                "\" with: " + std::make_error_code(ec).message()};
  }

  return result;
}

template <number T>
value_t magic_cast(const T& value) {
  constexpr auto buffer_size{std::is_integral_v<T> ? std::numeric_limits<T>::digits10 + 1 : 100};

  std::array<char, buffer_size> buffer;
  auto [ptr, ec]{std::to_chars(buffer.begin(), buffer.end(), value)};
  if (ec != std::errc{}) {
    throw std::invalid_argument{"Failed to convert value magic_cast type \"" + type_name<T>() +
                                "\" with: " + std::make_error_code(ec).message()};
  }

  return std::string{buffer.begin(), ptr};
}

}  // namespace sourcerer::detail
