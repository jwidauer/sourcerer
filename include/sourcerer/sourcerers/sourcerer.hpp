#pragma once

#include "sourcerer/node.hpp"

namespace sourcerer {

class Sourcerer {
 public:
  virtual ~Sourcerer() = default;

  virtual Node source() = 0;
};

}  // namespace sourcerer
