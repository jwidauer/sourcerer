#pragma once

#include <sourcerer/common.hpp>
#include <sourcerer/detail/convert.hpp>
#include <sourcerer/detail/types.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sourcerer {

// helper type for the visitor
template <typename... Ts>
struct Overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

class SOURCERER_API Node {
 public:
  using reference = Node&;
  using const_reference = const Node&;

  using index_t = detail::index_t;
  using key_t = detail::key_t;

  using null_t = detail::null_t;
  using value_t = detail::value_t;
  using array_t = detail::array_t;
  using object_t = detail::object_t;

  Node() = default;
  ~Node() = default;

  Node(const null_t& value);
  Node(const value_t& value);
  Node(const array_t& value);
  Node(const object_t& value);
  Node(const Node& other);

  Node(null_t&& value);
  Node(value_t&& value);
  Node(array_t&& value);
  Node(object_t&& value);
  Node(Node&& other) = default;

  Node(const Node* parent);
  Node(const Node* parent, const null_t& value);
  Node(const Node* parent, const value_t& value);
  Node(const Node* parent, const array_t& value);
  Node(const Node* parent, const object_t& value);

  Node(const Node* parent, const Node& other);

  constexpr bool is_null() const noexcept { return is<null_t>(); }

  constexpr bool is_value() const noexcept { return is<value_t>(); }

  constexpr bool is_object() const noexcept { return is<object_t>(); }

  constexpr bool is_array() const noexcept { return is<array_t>(); }

  reference at(const index_t index);
  const_reference at(const index_t index) const;

  reference at(const key_t& key);
  const_reference at(const key_t& key) const;

  reference operator[](const index_t index);
  const_reference operator[](const index_t index) const;

  reference operator[](const key_t& key);
  const_reference operator[](const key_t& key) const;

  void push_back(const value_t& value);
  void push_back(const Node& node);

  template <typename T>
  T as() const {
    return std::visit([](auto&& arg) { return detail::convert<T>(arg); }, children_);
  }

  // std::string to_string() const {
  //   return std::visit([](auto&& arg) { return detail::convert_s(arg); }, children_);
  // }

 private:
  template <class T>
  constexpr bool is() const noexcept {
    return std::holds_alternative<T>(children_);
  }

  std::string type_name() const noexcept {
    return std::visit(
        [](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          return detail::type_name<T>();
        },
        children_);
  }

  template <class T>
  auto& get() {
    try {
      return std::get<T>(children_);
    } catch (const std::exception& e) {
      throw std::runtime_error("Node is not an " + detail::type_name<T>());
    }
  }

  template <class T>
  const auto& get() const {
    try {
      return std::get<T>(children_);
    } catch (const std::exception& e) {
      throw std::runtime_error("Node is not an " + detail::type_name<T>());
    }
  }

  void copy(const null_t&) { children_.emplace<null_t>(); }
  void copy(const value_t& value) { children_.emplace<value_t>(value); }
  void copy(const array_t& value) {
    children_.emplace<array_t>();
    std::get<array_t>(children_).reserve(value.size());

    for (const auto& node : value) {
      std::get<array_t>(children_).push_back(std::make_unique<Node>(this, *node));
    }
  }
  void copy(const object_t& value) {
    children_.emplace<object_t>();

    for (const auto& [key, node] : value) {
      std::get<object_t>(children_).emplace(key, std::make_unique<Node>(this, *node));
    }
  }

  const Node* parent_ = nullptr;
  std::variant<null_t, value_t, array_t, object_t> children_;
};

}  // namespace sourcerer
