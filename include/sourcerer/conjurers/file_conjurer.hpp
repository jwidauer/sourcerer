#pragma once

#include <sourcerer/conjurers/conjurer.hpp>

#include <fstream>
#include <sstream>
#include <string>

namespace sourcerer::conjurers {

class FileConjurer : public Conjurer {
 public:
  FileConjurer(const std::string& path) : path_(path) {}

  inline std::string conjure() override {
    std::ifstream file(path_);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  };

 private:
  std::string path_;
};

}  // namespace sourcerer::conjurers
