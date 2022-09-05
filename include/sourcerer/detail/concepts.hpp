#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>
#include <variant>

namespace sourcerer::detail {

template <typename T>
concept number = std::is_arithmetic_v<T> && !
std::same_as<T, bool>;

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
