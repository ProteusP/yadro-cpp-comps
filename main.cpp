#include "ClubManager.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Failed to open file" << std::endl;
    return 1;
  }

  ClubManager manager;
  if (!manager.loadConfig(input)) {
    return 1;
  }
  std::string line;

  while (std::getline(input, line)) {
    if (!manager.processEvent(line)) {
      std::cout << line << std::endl;
      return 1;
    }
  }

  manager.finishDay();
  manager.printReport();

  return 0;
}