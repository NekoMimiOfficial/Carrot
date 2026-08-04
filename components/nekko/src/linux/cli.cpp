#include "cli.h"
#include <iostream>
#include <string>

void carrot_stdout(std::string msg, std::string terminator) {
  std::cout << msg << terminator;
}

void carrot_stderr(std::string msg, std::string terminator) {
  std::cerr << msg << terminator;
}
