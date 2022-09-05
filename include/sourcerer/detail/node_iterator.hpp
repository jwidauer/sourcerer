#pragma once

#include <sourcerer/common.hpp>
#include <sourcerer/detail/helpers.hpp>

#include <cassert>
#include <variant>

namespace sourcerer::detail {

/**
 * @brief A template for a bidirectional iterator for the @ref Node class.
 * This class implements both the @ref std::iterator concept and the @ref
 * std::bidirectional_iterator concept. As well as the const and non-const versions.
 */
template <basic_node BasicNode>
class iter_impl {
 public:
  friend BasicNode;

  using value_type = typename BasicNode::value_type;
  using difference_type = typename BasicNode::difference_type;
  using pointer = std::conditional_t<std::is_const_v<BasicNode>, typename BasicNode::const_pointer,
                                     typename BasicNode::pointer>;
  using reference =
      std::conditional_t<std::is_const_v<BasicNode>, typename BasicNode::const_reference,
                         typename BasicNode::reference>;

  iter_impl() = default;
  ~iter_impl() = default;
  iter_impl(const iter_impl&) = default;
  iter_impl& operator=(const iter_impl&) = default;
  iter_impl(iter_impl&&) noexcept = default;
  iter_impl& operator=(iter_impl&&) noexcept = default;

  explicit iter_impl(pointer node) : node_{node} {
    assert(node_ != nullptr);

    std::visit(
        overloaded{
            [&](auto&) { iter_.simple = 0; },
            [&](BasicNode::array_t&) { iter_.array = typename BasicNode::array_t::iterator{}; },
            [&](BasicNode::object_t&) { iter_.object = typename BasicNode::object_t::iterator{}; }},
        node_->children_);
  }

 private:
  void set_begin() {
    assert(node_ != nullptr);

    std::visit(overloaded{[&](auto&) -> void { iter_.simple = 1; },
                          [&](BasicNode::value_t&) -> void { iter_.simple = 0; },
                          [&](BasicNode::array_t& v) -> void { iter_.array = v.begin(); },
                          [&](BasicNode::object_t& v) -> void { iter_.object = v.begin(); }},
               node_->children_);
  }

  void set_end() {
    assert(node_ != nullptr);

    std::visit(overloaded{[&](auto&) { iter_.simple = 1; },
                          [&](BasicNode::array_t& v) { iter_.array = v.end(); },
                          [&](BasicNode::object_t& v) { iter_.object = v.end(); }},
               node_->children_);
  }

 public:
  pointer operator->() const {
    assert(node_ != nullptr);

    return std::visit(overloaded{[&](auto&) {
                                   if (iter_.simple != 0) throw std::out_of_range{""};
                                   return node_;
                                 },
                                 [&](BasicNode::array_t&) { return iter_.array->get(); },
                                 [&](BasicNode::object_t&) { return iter_.object->second.get(); }},
                      node_->children_);
  }

  reference operator*() const { return *operator->(); }

  iter_impl& operator++() {
    assert(node_ != nullptr);

    std::visit(overloaded{[&](auto&) {
                            if (iter_.simple != 0) throw std::out_of_range{""};
                            ++iter_.simple;
                          },
                          [&](BasicNode::array_t&) { ++iter_.array; },
                          [&](BasicNode::object_t&) { ++iter_.object; }},
               node_->children_);
    return *this;
  }

  iter_impl operator++(int) {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  iter_impl& operator--() {
    assert(node_ != nullptr);

    std::visit(overloaded{[&](auto&) {
                            if (iter_.simple != 1) throw std::out_of_range{""};
                            --iter_.simple;
                          },
                          [&](BasicNode::array_t&) { --iter_.array; },
                          [&](BasicNode::object_t&) { --iter_.object; }},
               node_->children_);
    return *this;
  }

  iter_impl operator--(int) {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  bool operator==(const iter_impl& other) const {
    if (node_ != other.node_) return false;

    assert(node_ != nullptr);

    return std::visit(
        overloaded{[&](auto&) { return iter_.simple == other.iter_.simple; },
                   [&](BasicNode::array_t&) { return iter_.array == other.iter_.array; },
                   [&](BasicNode::object_t&) { return iter_.object == other.iter_.object; }},
        node_->children_);
  }

  bool operator!=(const iter_impl& other) const { return !(*this == other); }

  bool operator<(const iter_impl& other) const {
    if (node_ != other.node_) return false;

    assert(node_ != nullptr);

    return std::visit(
        overloaded{[&](auto&) { return iter_.simple < other.iter_.simple; },
                   [&](BasicNode::array_t&) { return iter_.array < other.iter_.array; },
                   [&](BasicNode::object_t&) { return iter_.object < other.iter_.object; }},
        node_->children_);
  }

  bool operator>(const iter_impl& other) const { return other < *this; }

  bool operator<=(const iter_impl& other) const { return !(other < *this); }

  bool operator>=(const iter_impl& other) const { return !(*this < other); }

  difference_type operator-(const iter_impl& other) const {
    assert(node_ != nullptr);

    if (node_ != other.node_) throw std::out_of_range{""};
    return std::visit(
        overloaded{[&](auto&) { return iter_.simple - other.iter_.simple; },
                   [&](BasicNode::array_t&) { return iter_.array - other.iter_.array; },
                   [&](BasicNode::object_t&) { return iter_.object - other.iter_.object; }},
        node_->children_);
  }

  friend difference_type operator-(const iter_impl& it1, const iter_impl& it2) { return it2 - it1; }

 private:
  union iter_t {
    difference_type simple;
    typename BasicNode::array_t::iterator array;
    typename BasicNode::object_t::iterator object;
  } iter_{};
  pointer node_{nullptr};
};

}  // namespace sourcerer::detail