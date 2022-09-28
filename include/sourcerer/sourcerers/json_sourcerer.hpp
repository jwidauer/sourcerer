#pragma once

#include <nlohmann/json.hpp>

#include "sourcerer/conjurers/conjurer.hpp"
#include "sourcerer/node.hpp"
#include "sourcerer/sourcerers/sourcerer.hpp"

namespace sourcerer {

class JsonSourcerer : public Sourcerer {
 public:
  explicit JsonSourcerer(Conjurer& conjurer) {
    convert_to_node(root_, nlohmann::json::parse(conjurer.conjure()));
  }

  inline Node source() override { return root_; };

 private:
  void convert_to_node(Node& parent, const nlohmann::json& json) const {
    switch (json.type()) {
      case nlohmann::json::value_t::array:
        for (const auto& element : json) {
          Node child;
          convert_to_node(child, element);
          parent.push_back(child);
        }
        break;
      case nlohmann::json::value_t::object:
        for (const auto& [key, value] : json.items()) {
          Node child;
          convert_to_node(child, value);
          parent[key] = child;
        }
        break;
      default:
        parent = json.dump();
        break;
    }
  }

  Node root_;
};

}  // namespace sourcerer
