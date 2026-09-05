#pragma once
#include "value.h"
#include "utils/ctypeutils.h"
#include "utils/methods.h"
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

struct NumFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "num"; }
  Value call(std::vector<Value> args) override {
    if (std::holds_alternative<double>(args[0]))
      return args[0];
    if (std::holds_alternative<std::string>(args[0])) {
      try {
        return std::stod(std::get<std::string>(args[0]));
      } catch (...) {
      }
    }
    throw std::runtime_error("num(): cannot convert to number.");
  }
};

struct TypeFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "type"; }
  Value call(std::vector<Value> args) override {
    return getType(args[0]);
  }
};

struct LenFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "len"; }
  Value call(std::vector<Value> args) override {
    if (std::holds_alternative<std::string>(args[0]))
      return (double)std::get<std::string>(args[0]).size();
    if (std::holds_alternative<std::shared_ptr<NinArray>>(args[0]))
      return (double)std::get<std::shared_ptr<NinArray>>(args[0])
          ->elements.size();
    throw std::runtime_error("len(): argument must be a string or array.");
  }
};

