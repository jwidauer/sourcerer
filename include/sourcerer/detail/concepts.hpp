#pragma once

#include <concepts>

namespace sourcerer::detail {

template <typename T>
concept number = std::is_arithmetic_v<T> && !std::same_as<T, bool>;

}  // namespace sourcerer::detail
