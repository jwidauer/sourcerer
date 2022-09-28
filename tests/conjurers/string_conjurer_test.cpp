#include <sourcerer/conjurers/string_conjurer.hpp>

#include <doctest.h>

TEST_SUITE_BEGIN("[StringConjurer]");

TEST_CASE("Conjure") {
  const std::string string = "Hello, world!";
  sourcerer::StringConjurer conjurer(string);
  CHECK(conjurer.conjure() == string);
}

TEST_SUITE_END();
