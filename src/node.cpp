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

Node::Node(const Node* parent, const array_t& value) : parent_{parent} {
  children_.emplace<array_t>();
  std::get<array_t>(children_).reserve(value.size());

  for (const auto& child : value) {
    std::get<array_t>(children_).emplace_back(std::make_unique<Node>(this, *child));
  }
}

Node::Node(const Node* parent, const object_t& value) : parent_{parent} {
  children_.emplace<object_t>();

  for (const auto& [key, val] : value) {
    std::get<object_t>(children_).emplace(key, std::make_unique<Node>(this, *val));
  }
}

Node::Node(const Node* parent, const Node& other) : parent_{parent} {
  if (other.is_null()) {
    children_ = other.get<null_t>();
  } else if (other.is_value()) {
    children_ = other.get<value_t>();
  } else if (other.is_object()) {
    children_.emplace<object_t>();
    for (const auto& [key, value] : other.get<object_t>()) {
      std::get<object_t>(children_).emplace(key, std::make_unique<Node>(this, *value));
    }
  } else if (other.is_array()) {
    children_.emplace<array_t>();
    std::get<array_t>(children_).reserve(other.get<array_t>().size());

    for (const auto& value : other.get<array_t>()) {
      std::get<array_t>(children_).push_back(std::make_unique<Node>(this, *value));
    }
  } else {
    assert(false);
  }
}

Node::reference Node::at(const index_t index) { return *(get<array_t>().at(index)); }

Node::const_reference Node::at(const index_t index) const { return *(get<array_t>().at(index)); }

Node::reference Node::at(const key_t& key) { return *(get<object_t>().at(key)); }

Node::const_reference Node::at(const key_t& key) const { return *(get<object_t>().at(key)); }

Node::reference Node::operator[](const index_t index) { return *(get<array_t>()[index]); }

Node::const_reference Node::operator[](const index_t index) const {
  return *(get<array_t>()[index]);
}

Node::reference Node::operator[](const key_t& key) { return *(get<object_t>()[key]); }

Node::const_reference Node::operator[](const key_t& key) const {
  if (is_object()) {
    auto it = std::get<object_t>(children_).find(key);
    assert(it != std::get<object_t>(children_).end());
    return *(it->second);
  }

  throw std::runtime_error("Node is not an object");
}

void Node::push_back(const value_t& value) {
  if (!(is_null() || is_array())) {
    throw std::runtime_error("Can't push_back to a node of type " + type_name());
  }

  if (is_null()) {
    children_.emplace<array_t>();
  }

  std::get<array_t>(children_).push_back(std::make_unique<Node>(this, value));
}

void Node::push_back(const Node& node) {
  if (!(is_null() || is_array())) {
    throw std::runtime_error("Can't push_back on a node of type " + type_name());
  }

  if (is_null()) {
    children_.emplace<array_t>();
  }

  std::get<array_t>(children_).push_back(std::make_unique<Node>(this, node));
}

}  // namespace sourcerer
