#include <sourcerer/conjurers/file_conjurer.hpp>

#include <cstdio>
#include <doctest.h>
#include <filesystem>

TEST_SUITE_BEGIN("[FileConjurer]");

TEST_CASE("Conjure") {
  const auto tmp_file{std::tmpfile()};
  CHECK(tmp_file != nullptr);

  std::fputs("Hello, world!", tmp_file);
  std::rewind(tmp_file);

  const auto path = std::filesystem::path{"/proc/self/fd/"} / std::to_string(fileno(tmp_file));

  sourcerer::FileConjurer conjurer(std::string{path});
  CHECK(conjurer.conjure() == "Hello, world!");
}

TEST_CASE("Conjure non-existent file") {
  const auto path = std::filesystem::path{"/tmp/"} / std::to_string(std::rand());
  sourcerer::FileConjurer conjurer(std::string{path});
  CHECK_THROWS_AS(conjurer.conjure(), std::runtime_error);
}

TEST_CASE("Conjure directory") {
  const auto path = std::filesystem::path{"/tmp/"};
  sourcerer::FileConjurer conjurer(std::string{path});
  CHECK_THROWS_AS(conjurer.conjure(), std::runtime_error);
}

TEST_SUITE_END();
