#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <sourcerer/node.hpp>

#include "doctest.h"

TEST_SUITE_BEGIN("[Node]");
using namespace sourcerer;

TYPE_TO_STRING(Node);

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

TEST_CASE_TEMPLATE("Array accessors", T, Node, const Node) {
  Node init_node;
  init_node.push_back("value");

  T node{init_node};

  SUBCASE("at") {
    CHECK(node.at(0).is_value());
    CHECK(node.at(0) == Node{"value"});
  }

  SUBCASE("operator[]") {
    CHECK(node[0].is_value());
    CHECK(node[0] == Node{"value"});
  }

  SUBCASE("at should throw if index is out of bounds") {
    CHECK_THROWS_AS(node.at(1), std::out_of_range);
  }

  SUBCASE("at should throw if index is negative") {
    CHECK_THROWS_AS(node.at(-1), std::out_of_range);
  }

  SUBCASE("at should throw with non-integral index") {
    CHECK_THROWS_AS(node.at("key"), std::invalid_argument);
  }

  SUBCASE("operator[] should throw with non-integral index") {
    CHECK_THROWS_AS(node["key"], std::invalid_argument);
  }
}

TEST_CASE("Array modifiers") {
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

    SUBCASE("erase should throw if index is out of bounds") {
      CHECK_THROWS_AS(node.erase(1), std::out_of_range);
    }

    SUBCASE("erase should throw if index is negative") {
      CHECK_THROWS_AS(node.erase(-1), std::out_of_range);
    }

    SUBCASE("erase should throw with non-integral index") {
      CHECK_THROWS_AS(node.erase("key"), std::invalid_argument);
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

TEST_CASE_TEMPLATE("Object accessors", T, Node, const Node) {
  Node init_node;
  init_node["key"] = "value";

  T node{init_node};

  SUBCASE("at") {
    CHECK(node.at("key").is_value());
    CHECK(node.at("key") == Node{"value"});
  }

  SUBCASE("operator[]") {
    CHECK(node["key"].is_value());
    CHECK(node["key"] == Node{"value"});
  }

  SUBCASE("at should throw if key is not found") {
    CHECK_THROWS_AS(node.at("key2"), std::out_of_range);
  }

  SUBCASE("at should throw with non-string key") {
    CHECK_THROWS_AS(node.at(0), std::invalid_argument);
  }

  SUBCASE("operator[] should throw with non-string key") {
    CHECK_THROWS_AS(node[0], std::invalid_argument);
  }
}

TEST_CASE("Object modifiers") {
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

    SUBCASE("erase should throw with non-string key") {
      CHECK_THROWS_AS(node.erase(0), std::invalid_argument);
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

  SUBCASE("value") {
    node = "value";
    CHECK(!node.empty());
    CHECK(node.size() == 1);
  }
}

TEST_CASE("Operators") {
  SUBCASE("operator==") {
    CHECK(Node{"value"} == Node{"value"});
    CHECK_FALSE(Node{"value"} == Node{"value2"});
  }

  SUBCASE("operator!=") {
    CHECK_FALSE(Node{"value"} != Node{"value"});
    CHECK(Node{"value"} != Node{"value2"});
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

TEST_CASE("Iterator") {
  Node node;
  node.push_back("value1");
  node.push_back("value2");
  node.push_back("value3");

  SUBCASE("begin") {
    auto it = node.begin();
    CHECK(it->as<Node::value_t>() == "value1");
  }

  SUBCASE("end") {
    auto begin = node.begin();
    auto end = node.end();
    CHECK(begin != end);

    begin++;
    begin++;
    begin++;
    CHECK(begin == end);
  }

  SUBCASE("Accessing") {
    auto it = node.begin();

    SUBCASE("operator->") {
      CHECK(it->as<Node::value_t>() == "value1");
      it++;
      CHECK(it->as<Node::value_t>() == "value2");
      it++;
      CHECK(it->as<Node::value_t>() == "value3");
    }

    SUBCASE("operator*") {
      CHECK((*it).as<Node::value_t>() == "value1");
      it++;
      CHECK((*it).as<Node::value_t>() == "value2");
      it++;
      CHECK((*it).as<Node::value_t>() == "value3");
    }
  }

  SUBCASE("Increment") {
    auto it = node.begin();

    SUBCASE("operator++") {
      CHECK(it->as<Node::value_t>() == "value1");
      ++it;
      CHECK(it->as<Node::value_t>() == "value2");
      ++it;
      CHECK(it->as<Node::value_t>() == "value3");
    }

    SUBCASE("operator++(int)") {
      CHECK(it->as<Node::value_t>() == "value1");
      it++;
      CHECK(it->as<Node::value_t>() == "value2");
      it++;
      CHECK(it->as<Node::value_t>() == "value3");
    }
  }

  SUBCASE("Decrement") {
    auto it = node.end();

    SUBCASE("operator--") {
      --it;
      CHECK(it->as<Node::value_t>() == "value3");
      --it;
      CHECK(it->as<Node::value_t>() == "value2");
      --it;
      CHECK(it->as<Node::value_t>() == "value1");
    }

    SUBCASE("operator--(int)") {
      it--;
      CHECK(it->as<Node::value_t>() == "value3");
      it--;
      CHECK(it->as<Node::value_t>() == "value2");
      it--;
      CHECK(it->as<Node::value_t>() == "value1");
    }
  }

  SUBCASE("Comparison") {
    auto it1 = node.begin();
    auto it2 = node.begin();

    SUBCASE("operator==") {
      CHECK(it1 == it2);
      it1++;
      CHECK_FALSE(it1 == it2);
    }

    SUBCASE("operator!=") {
      CHECK_FALSE(it1 != it2);
      it1++;
      CHECK(it1 != it2);
    }
  }
}

TEST_SUITE_END();
