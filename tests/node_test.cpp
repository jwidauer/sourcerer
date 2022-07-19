#include <sourcerer/node.hpp>

#include <iostream>

int main() {
  using namespace sourcerer;

  Node root;
  root.push_back("hello");
  root.push_back("10");

  //   std::cout << root.to_string() << std::endl;

  std::cout << root.at(0).as<std::string>() << std::endl;
  std::cout << root.at(1).as<int>() << std::endl;

  return 0;
}