#pragma once

#include <string>

#include "sourcerer/conjurers/conjurer.hpp"

namespace sourcerer {

/**
 * @brief A conjurer that conjures data from an input string.
 *
 * It's main use case is that of testing.
 */
class StringConjurer : public Conjurer {
 public:
  explicit StringConjurer(std::string string) : string_(std::move(string)) {}

  inline std::string conjure() override { return string_; }

 private:
  std::string string_;
};

}  // namespace sourcerer
