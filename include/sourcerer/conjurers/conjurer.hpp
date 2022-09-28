#pragma once

#include <string>

namespace sourcerer {

class Conjurer {
 public:
  virtual ~Conjurer() = default;

  virtual std::string conjure() = 0;
};

}  // namespace sourcerer
