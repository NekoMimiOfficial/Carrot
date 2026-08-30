#include "value.h"
#include <iostream>
#include <variant>
#include <vector>

#include "platform_module.h"

struct LinuxFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "__builtin_fn_7400"; }
  Value call(std::vector<Value>) override {
    std::cout
        << "DOOM?? pfft- the new benchmark is \"Does it run Carrot\" :3c\n";
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

struct PrintFn : NinCallable {
  int arity() override { return 0; }
  bool isVariadic() override { return true; }
  std::string name() override { return "print"; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
  }

  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    std::string end = "\n";
    auto it = kwargs.find("end");
    if (it != kwargs.end()) {
      if (!std::holds_alternative<std::string>(it->second))
        throw std::runtime_error("print(): 'end' must be a string.");
      end = std::get<std::string>(it->second);
      kwargs.erase(it);
    }
    if (!kwargs.empty())
      throw std::runtime_error("print(): unknown keyword argument '" +
                               kwargs.begin()->first + "'.");

    if (args.empty() || !std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error(
          "print(): first argument must be a format string.");

    const std::string &fmt = std::get<std::string>(args[0]);
    size_t argIndex = 1;
    std::string out;
    for (size_t i = 0; i < fmt.size(); i++) {
      if (fmt[i] == '{' && i + 1 < fmt.size() && fmt[i + 1] == '}') {
        if (argIndex >= args.size())
          throw std::runtime_error(
              "print(): not enough arguments for format string.");
        out += valueToString(args[argIndex++]);
        i++;
      } else {
        out += fmt[i];
      }
    }
    std::cout << out << end;
    return std::monostate{};
  }
};
