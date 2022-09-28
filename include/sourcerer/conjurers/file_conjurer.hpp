#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "sourcerer/conjurers/conjurer.hpp"

namespace sourcerer {

class FileConjurer : public Conjurer {
 public:
  explicit FileConjurer(const std::string& path) : path_{path} {}

  inline std::string conjure() override {
    if (!std::filesystem::exists(path_)) {
      throw std::runtime_error("File does not exist: " + std::string{path_});
    }
    if (!std::filesystem::is_regular_file(path_)) {
      throw std::runtime_error("Path is not a file: " + std::string{path_});
    }

    std::ifstream file{path_};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  };

 private:
  std::filesystem::path path_;
};

}  // namespace sourcerer
