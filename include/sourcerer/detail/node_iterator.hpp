#pragma once

#include <cassert>
#include <compare>
#include <variant>

#include "sourcerer/common.hpp"
#include "sourcerer/detail/helpers.hpp"

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

    std::visit(detail::overloaded{[this](auto&) { iter_.simple = 0; },
                                  [this](BasicNode::array_t&) {
                                    iter_.array = typename BasicNode::array_t::iterator{};
                                  },
                                  [this](BasicNode::object_t&) {
                                    iter_.object = typename BasicNode::object_t::iterator{};
                                  }},
               node_->children_);
  }

 private:
  void set_begin() {
    assert(node_ != nullptr);

    std::visit(detail::overloaded{[this](auto&) { iter_.simple = 1; },
                                  [this](BasicNode::value_t&) { iter_.simple = 0; },
                                  [this](BasicNode::array_t& v) { iter_.array = std::begin(v); },
                                  [this](BasicNode::object_t& v) { iter_.object = std::begin(v); }},
               node_->children_);
  }

  void set_end() {
    assert(node_ != nullptr);

    std::visit(detail::overloaded{[this](auto&) { iter_.simple = 1; },
                                  [this](BasicNode::array_t& v) { iter_.array = std::end(v); },
                                  [this](BasicNode::object_t& v) { iter_.object = std::end(v); }},
               node_->children_);
  }

 public:
  pointer operator->() const {
    assert(node_ != nullptr);

    return std::visit(
        detail::overloaded{[this](auto&) {
                             if (iter_.simple != 0) throw std::out_of_range{""};
                             return node_;
                           },
                           [this](BasicNode::array_t&) { return iter_.array->get(); },
                           [this](BasicNode::object_t&) { return iter_.object->second.get(); }},
        node_->children_);
  }

  reference operator*() const { return *operator->(); }

  iter_impl& operator++() {
    assert(node_ != nullptr);

    std::visit(detail::overloaded{[this](auto&) {
                                    if (iter_.simple != 0) throw std::out_of_range{""};
                                    ++iter_.simple;
                                  },
                                  [this](BasicNode::array_t&) { ++iter_.array; },
                                  [this](BasicNode::object_t&) { ++iter_.object; }},
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

    std::visit(detail::overloaded{[this](auto&) {
                                    if (iter_.simple != 1) throw std::out_of_range{""};
                                    --iter_.simple;
                                  },
                                  [this](BasicNode::array_t&) { --iter_.array; },
                                  [this](BasicNode::object_t&) { --iter_.object; }},
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
        detail::overloaded{
            [this, &other](auto&) { return iter_.simple == other.iter_.simple; },
            [this, &other](BasicNode::array_t&) { return iter_.array == other.iter_.array; },
            [this, &other](BasicNode::object_t&) { return iter_.object == other.iter_.object; }},
        node_->children_);
  }

 private:
  union iter_t {
    difference_type simple;
    typename BasicNode::array_t::iterator array;
    typename BasicNode::object_t::iterator object;
  };
  iter_t iter_{};
  pointer node_{nullptr};
};

}  // namespace sourcerer::detail
