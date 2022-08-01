#include <sourcerer/detail/helpers.hpp>
#include <sourcerer/node.hpp>

#include <cassert>
#include <memory>

namespace sourcerer {

Node::Node(const null_t&) : Node() {}

Node::Node(const value_t& value) : Node(nullptr, value) {}

Node::Node(const array_t& value) : Node(nullptr, value) {}

Node::Node(const object_t& value) : Node(nullptr, value) {}

Node::Node(const Node& other) : Node(nullptr, other) {}

Node::Node(null_t&&) : Node() {}

Node::Node(value_t&& value) : parent_{nullptr}, children_{std::move(value)} {}

Node::Node(array_t&& value) : parent_{nullptr}, children_{std::move(value)} {}

Node::Node(object_t&& value) : parent_{nullptr}, children_{std::move(value)} {}

Node::Node(const Node* parent) : parent_{parent} {}

Node::Node(const Node* parent, const null_t& value) : parent_{parent}, children_{value} {}

Node::Node(const Node* parent, const value_t& value) : parent_{parent}, children_{value} {}

Node::Node(const Node* parent, const array_t& value) : parent_{parent} { copy(value); }

Node::Node(const Node* parent, const object_t& value) : parent_{parent} { copy(value); }

Node::Node(const Node* parent, const Node& other) : parent_{parent} {
  std::visit([this](auto&& v) { this->copy(v); }, other.children_);
}

Node::reference Node::at(const size_type index) { return *(get<array_t>().at(index)); }

Node::const_reference Node::at(const size_type index) const { return *(get<array_t>().at(index)); }

Node::reference Node::at(const key_type& key) { return *(get<object_t>().at(key)); }

Node::const_reference Node::at(const key_type& key) const { return *(get<object_t>().at(key)); }

Node::reference Node::operator[](const size_type index) { return *(get<array_t>()[index]); }

Node::const_reference Node::operator[](const size_type index) const {
  return *(get<array_t>()[index]);
}

Node::reference Node::operator[](const key_type& key) {
  prepare_for<object_t>("Can't use operator[] on a node of type " + type_name());

  auto result = get<object_t>().try_emplace(key, create_from(Node{}));
  return *(result.first->second);
}

Node::const_reference Node::operator[](const key_type& key) const {
  if (!is_object()) {
    throw std::runtime_error("Can't use operator[] on a node of type " + type_name());
  }

  auto it = std::get<object_t>(children_).find(key);
  assert(it != std::get<object_t>(children_).end());
  return *(it->second);
}

void Node::push_back(const Node& node) {
  prepare_for<array_t>("Can't push_back on a node of type " + type_name());

  std::get<array_t>(children_).push_back(create_from(node));
}

void Node::emplace_back(const Node& node) {
  prepare_for<array_t>("Can't emplace_back on a node of type " + type_name());

  std::get<array_t>(children_).emplace_back(create_from(node));
}

void Node::erase(const size_type index) {
  if (!is_array()) {
    throw std::runtime_error("Can't erase with index on a node of type " + type_name());
  }

  std::get<array_t>(children_).erase(std::get<array_t>(children_).begin() + index);
}

void Node::erase(const key_type& key) {
  if (!is_object()) {
    throw std::runtime_error("Can't erase with key on a node of type " + type_name());
  }

  std::get<object_t>(children_).erase(key);
}

void Node::clear() {
  std::visit(
      overloaded{[](null_t&) { throw std::runtime_error{"Can't clear a node of type null"}; },
                 [](auto&& v) { v.clear(); }},
      children_);
}

bool Node::empty() const noexcept {
  return std::visit(
      overloaded{[](const null_t&) { return true; }, [](const auto& v) { return v.empty(); }},
      children_);
}

Node::size_type Node::size() const noexcept {
  return std::visit(overloaded{[](const null_t&) { return Node::size_type{}; },
                               [](const auto& v) { return v.size(); }},
                    children_);
}

void Node::insert(const size_type index, const Node& node) {
  prepare_for<array_t>("Can't insert with index on a node of type " + type_name());

  std::get<array_t>(children_).insert(std::get<array_t>(children_).begin() + index,
                                      create_from(node));
}

void Node::insert(const key_type& key, const Node& node) {
  prepare_for<object_t>("Can't insert with key on a node of type " + type_name());

  std::get<object_t>(children_).insert({key, create_from(node)});
}

void Node::emplace(const size_type index, const Node& node) {
  prepare_for<array_t>("Can't emplace with index on a node of type " + type_name());

  std::get<array_t>(children_).emplace(std::get<array_t>(children_).begin() + index,
                                       create_from(node));
}

void Node::emplace(const key_type& key, const Node& node) {
  prepare_for<object_t>("Can't emplace with key on a node of type " + type_name());

  std::get<object_t>(children_).emplace(key, create_from(node));
}

Node::reference Node::operator=(const Node& other) {
  if (this != &other) {
    std::visit([this](auto&& v) { this->copy(v); }, other.children_);
  }

  return *this;
}

void Node::swap(Node& other) noexcept {
  std::swap(parent_, other.parent_);
  children_.swap(other.children_);
}

}  // namespace sourcerer
