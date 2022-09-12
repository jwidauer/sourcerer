#pragma once

#include <string>

namespace sourcerer::conjurers {

class Conjurer {
 public:
  virtual ~Conjurer() = default;

  virtual std::string conjure() = 0;
};

}  // namespace sourcerer::conjurers
