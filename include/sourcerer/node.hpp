#pragma once

#include <sourcerer/common.hpp>
#include <sourcerer/detail/concepts.hpp>
#include <sourcerer/detail/from.hpp>
#include <sourcerer/detail/node_iterator.hpp>
#include <sourcerer/detail/types.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace sourcerer {

class SOURCERER_API Node {
 private:
  template <detail::basic_node NodeType>
  friend class detail::iter_impl;

 public:
  // the type of elements in a Node container
  using value_type = Node;
  using const_value_type = const Node;

  // the type of an element reference
  using reference = value_type&;
  // the type of an element const reference
  using const_reference = const value_type&;

  // a type to represent differences between iterators
  using difference_type = std::ptrdiff_t;
  // a type to represent container sizes
  using size_type = std::size_t;

  using key_type = detail::key_t;

  using null_t = detail::null_t;
  using value_t = detail::value_t;
  using array_t = detail::array_t;
  using object_t = detail::object_t;

  // the allocator type
  using allocator_type = std::allocator<Node>;

  // the type of an element pointer
  using pointer = typename std::allocator_traits<allocator_type>::pointer;
  // the type of an element const pointer
  using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

  // an iterator for a Node container
  using iterator = detail::iter_impl<Node>;
  static_assert(std::bidirectional_iterator<iterator>);

  // a const iterator for a Node container
  using const_iterator = detail::iter_impl<const Node>;
  static_assert(std::bidirectional_iterator<const_iterator>);

  // a reverse iterator for a Node container
  // using reverse_iterator = detail::reverse_iterator<typename Node::iterator>;
  // // a const reverse iterator for a Node container
  // using const_reverse_iterator = detail::reverse_iterator<typename Node::const_iterator>;

  Node() = default;
  ~Node() = default;

  explicit Node(const null_t& value);
  explicit Node(const value_t& value);
  explicit Node(const array_t& value);
  explicit Node(const object_t& value);
  Node(const Node& other);

  explicit Node(null_t&& value);
  explicit Node(value_t&& value);
  explicit Node(array_t&& value);
  explicit Node(object_t&& value);
  Node(Node&& other) = default;

  explicit Node(const Node* parent);
  Node(const Node* parent, const null_t& value);
  Node(const Node* parent, const value_t& value);
  Node(const Node* parent, const array_t& value);
  Node(const Node* parent, const object_t& value);
  Node(const Node* parent, const Node& other);

  constexpr bool is_null() const noexcept { return is<null_t>(); }
  constexpr bool is_value() const noexcept { return is<value_t>(); }
  constexpr bool is_object() const noexcept { return is<object_t>(); }
  constexpr bool is_array() const noexcept { return is<array_t>(); }

  template <class T>
  constexpr bool is() const noexcept {
    return std::holds_alternative<T>(children_);
  }

  auto parent() const noexcept { return parent_; }
  bool has_parent() const noexcept { return parent_ != nullptr; }

  reference at(const size_type index);
  const_reference at(const size_type index) const;

  reference at(const key_type& key);
  const_reference at(const key_type& key) const;

  reference operator[](const size_type index);
  const_reference operator[](const size_type index) const;

  reference operator[](const key_type& key);
  const_reference operator[](const key_type& key) const;

  void push_back(const Node& node);

  template <class T>
  void push_back(const T& value) {
    push_back(Node{detail::from(value)});
  }

  void emplace_back(const Node& node);

  template <class T>
  void emplace_back(const T& value) {
    emplace_back(Node{detail::from(value)});
  }

  void erase(const size_type index);
  void erase(const key_type& key);

  size_type size() const noexcept;
  bool empty() const noexcept;
  void clear();

  void insert(const size_type index, const Node& node);

  template <class T>
  void insert(const size_type index, const T& value) {
    insert(index, Node{detail::from(value)});
  }

  void emplace(const size_type index, const Node& node);

  template <class T>
  void emplace(const size_type index, const T& value) {
    emplace(index, Node{detail::from(value)});
  }

  void insert(const key_type& key, const Node& node);

  template <class T>
  void insert(const key_type& key, const T& value) {
    insert(key, Node{detail::from(value)});
  }

  void emplace(const key_type& key, const Node& node);

  template <class T>
  void emplace(const key_type& key, const T& value) {
    emplace(key, Node{detail::from(value)});
  }

  reference operator=(const Node& other);
  reference operator=(Node&& other) = default;

  template <class T>
  reference operator=(const T& value) {
    return operator=(Node{detail::from(value)});
  }

  bool operator==(const Node& other) const;

  void swap(Node& other) noexcept;

  template <typename T>
  T as() const {
    return std::visit([](const auto& arg) { return detail::from<T>(arg); }, children_);
  }

  iterator begin() noexcept;
  iterator end() noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  // std::string to_string() const {
  //   return std::visit([](auto&& arg) { return detail::convert_s(arg); }, children_);
  // }

 private:
  std::string type_name() const noexcept {
    return std::visit(
        overloaded{[](const null_t&) { return "null"; }, [](const value_t&) { return "value"; },
                   [](const array_t&) { return "array"; },
                   [](const object_t&) { return "object"; }},
        children_);
  }

  template <class T>
  auto& get() {
    try {
      return std::get<T>(children_);
    } catch (const std::exception& e) {
      throw std::invalid_argument("Node is not an " + detail::type_name<T>());
    }
  }

  template <class T>
  const auto& get() const {
    try {
      return std::get<T>(children_);
    } catch (const std::exception& e) {
      throw std::invalid_argument("Node is not an " + detail::type_name<T>());
    }
  }

  void copy(const null_t&) { children_.emplace<null_t>(); }
  void copy(const value_t& value) { children_.emplace<value_t>(value); }
  void copy(const array_t& value) {
    children_.emplace<array_t>();
    std::get<array_t>(children_).reserve(value.size());

    std::ranges::transform(
        value, std::back_inserter(std::get<array_t>(children_)),
        [this](const auto& node) { return std::make_unique<Node>(this, *node); });
  }
  void copy(const object_t& value) {
    children_.emplace<object_t>();

    for (const auto& [key, node] : value) {
      std::get<object_t>(children_).try_emplace(key, std::make_unique<Node>(this, *node));
    }
  }

  std::unique_ptr<Node> create_from(const Node& other) {
    return std::make_unique<Node>(this, other);
  }

  template <class T>
  void prepare_for(const std::string& error_msg) {
    if (!(is_null() || is<T>())) {
      throw std::invalid_argument(error_msg);
    }

    if (is_null()) {
      children_.emplace<T>();
    }
  }

  const Node* parent_ = nullptr;
  std::variant<null_t, value_t, array_t, object_t> children_;

  static_assert(detail::basic_node<Node>);
};

}  // namespace sourcerer
