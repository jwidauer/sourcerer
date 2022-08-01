#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <sourcerer/node.hpp>

#include "doctest.h"

TEST_SUITE_BEGIN("[Node]");
using namespace sourcerer;

TEST_CASE("Simple constructors") {
  SUBCASE("default constructor") {
    Node node;
    CHECK(node.is_null());
  }

  SUBCASE("Copy constructor") {
    Node node1{Node::array_t{}};
    Node node2(node1);
    CHECK(node2.is_array());
  }

  SUBCASE("Default constructor with parent") {
    Node node1;
    Node node2(&node1);
    CHECK(node2.is_null());
    CHECK(node2.parent() == &node1);
  }

  SUBCASE("Copy constructor with parent") {
    Node parent{Node::object_t{}};
    Node node{&parent, "value"};
    Node copy{&parent, node};
    CHECK(copy.is_value());
    CHECK(copy.as<Node::value_t>() == "value");
  }
}

TEST_CASE_TEMPLATE("Move constructors", T, Node::null_t, Node::value_t, Node::array_t,
                   Node::object_t) {
  SUBCASE("Value copy constructor") {
    const T value{};
    Node node{value};
    CHECK(node.is<T>());
    CHECK(!node.has_parent());
  }

  SUBCASE("Value move constructor") {
    Node node{T{}};
    CHECK(node.is<T>());
    CHECK(!node.has_parent());
  }

  SUBCASE("Move constructor") {
    Node node{T{}};
    Node copy{std::move(node)};
    CHECK(copy.is<T>());
    CHECK(!node.has_parent());
  }

  SUBCASE("Copy constructor with parent") {
    Node parent{Node::object_t{}};
    Node node{&parent, Node::null_t{}};
    Node copy{&parent, node};
    CHECK(copy.is_null());
    CHECK(copy.parent() == &parent);
  }
}

TEST_CASE("Array accessors") {
  using namespace sourcerer;
  Node node;
  node.push_back("value");

  SUBCASE("at") {
    CHECK(node.at(0).is_value());
    CHECK(node.at(0).as<Node::value_t>() == "value");
  }

  SUBCASE("operator[]") {
    CHECK(node[0].is_value());
    CHECK(node[0].as<Node::value_t>() == "value");
  }
}

TEST_CASE("Array modifiers") {
  using namespace sourcerer;
  Node node;

  SUBCASE("emplace_back") {
    node.emplace_back("value");
    CHECK(node[0].is_value());
    CHECK(node[0].as<Node::value_t>() == "value");
  }

  SUBCASE("insert") {
    node.insert(0, "value");
    CHECK(node[0].is_value());
    CHECK(node[0].as<Node::value_t>() == "value");
  }

  SUBCASE("emplace") {
    node.emplace(0, "value");
    CHECK(node[0].is_value());
    CHECK(node[0].as<Node::value_t>() == "value");
  }

  SUBCASE("push_back initialized") {
    node.push_back("value");

    SUBCASE("push_back") {
      CHECK(node[0].is_value());
      CHECK(node[0].as<Node::value_t>() == "value");
    }

    SUBCASE("erase") {
      node.erase(0);
      CHECK(node.empty());
    }

    SUBCASE("clear") {
      node.clear();
      CHECK(node.empty());
    }

    SUBCASE("operator=") {
      Node copy;
      copy = node;
      CHECK(copy[0].is_value());
      CHECK(copy[0].as<Node::value_t>() == "value");
    }

    SUBCASE("operator=") {
      Node copy;
      copy = std::move(node);
      CHECK(copy[0].is_value());
      CHECK(copy[0].as<Node::value_t>() == "value");
    }
  }
}

TEST_CASE("Object accessors") {
  using namespace sourcerer;
  Node node;
  node["key"] = "value";

  SUBCASE("at") {
    CHECK(node.at("key").is_value());
    CHECK(node.at("key").as<Node::value_t>() == "value");
  }

  SUBCASE("operator[]") {
    CHECK(node["key"].is_value());
    CHECK(node["key"].as<Node::value_t>() == "value");
  }
}

TEST_CASE("Object modifiers") {
  using namespace sourcerer;
  Node node;

  SUBCASE("insert") {
    node.insert("key", "value");
    CHECK(node["key"].is_value());
    CHECK(node["key"].as<Node::value_t>() == "value");
  }

  SUBCASE("emplace") {
    node.emplace("key", "value");
    CHECK(node["key"].is_value());
    CHECK(node["key"].as<Node::value_t>() == "value");
  }

  SUBCASE("Value initialized") {
    node["key"] = "value";

    SUBCASE("erase") {
      node.erase("key");
      CHECK(node.empty());
    }

    SUBCASE("clear") {
      node.clear();
      CHECK(node.empty());
    }

    SUBCASE("operator=") {
      Node copy;
      copy = node;
      CHECK(copy["key"].is_value());
      CHECK(copy["key"].as<Node::value_t>() == "value");
    }

    SUBCASE("operator=") {
      Node copy;
      copy = std::move(node);
      CHECK(copy["key"].is_value());
      CHECK(copy["key"].as<Node::value_t>() == "value");
    }
  }
}

TEST_CASE("Size") {
  Node node;

  SUBCASE("empty") { CHECK(node.empty()); }

  SUBCASE("size") { CHECK(node.size() == 0); }

  SUBCASE("push_back") {
    node.push_back("value");
    CHECK(!node.empty());
    CHECK(node.size() == 1);
  }

  SUBCASE("push_back many") {
    node.push_back("value");
    node.push_back("value");
    node.push_back("value");
    CHECK(!node.empty());
    CHECK(node.size() == 3);
  }
}

TEST_CASE("Swap") {
  Node node1;
  node1.push_back("value1");
  Node node2;
  node2.push_back("value2");
  node1.swap(node2);
  CHECK(node1[0].as<Node::value_t>() == "value2");
  CHECK(node2[0].as<Node::value_t>() == "value1");
}

TEST_SUITE_END();
