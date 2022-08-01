#pragma once

// Helper for static asserts.
template <class>
inline constexpr bool always_false_v = false;

// Helper type for visitors
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
