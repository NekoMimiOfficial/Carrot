#include "value.h"
#include <iostream>
#include <variant>
#include <vector>

#include "platform_module.h"

struct LinuxFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "__builtin_fn_7400"; }
  Value call(std::vector<Value>) override {
    std::cout << "DOOM?? pfft- the new benchmark is \"Does it run Carrot\" :3c\n";
    return std::monostate{};
  }
};

struct InputFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "input"; }
  Value call(std::vector<Value> args) override {
    std::cout << valueToString(args[0]);
    std::string line;
    std::getline(std::cin, line);
    return line;
  }
};


