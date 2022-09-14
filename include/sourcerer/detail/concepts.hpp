#pragma once

#include <sourcerer/detail/node_forwards.hpp>

#include <concepts>
#include <iterator>
#include <type_traits>
#include <variant>

namespace sourcerer::detail {

template <class T>
concept number = std::is_arithmetic_v<T> && !std::same_as<T, bool>;

template <class T>
concept child = std::same_as<T, null_t> || std::same_as<T, value_t> || std::same_as<T, array_t> ||
                std::same_as<T, object_t>;

template <class T>
concept child_variant = std::same_as<T, std::variant<null_t, value_t, array_t, object_t>>;

template <class T>
concept basic_node =
    requires(T t) {
      typename T::difference_type;
      typename T::null_t;
      typename T::value_t;
      typename T::array_t;
      typename T::object_t;
      requires std::same_as<typename T::pointer, std::remove_const_t<T>*>;
      requires std::same_as<typename T::const_pointer, const T*>;
      requires std::same_as<typename T::reference, std::remove_const_t<T>&>;
      requires std::same_as<typename T::const_reference, const T&>;
    } && std::bidirectional_iterator<typename T::array_t::iterator> &&
    std::bidirectional_iterator<typename T::object_t::iterator>;

}  // namespace sourcerer::detail
