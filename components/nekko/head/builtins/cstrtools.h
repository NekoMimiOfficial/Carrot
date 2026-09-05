#pragma once
#include "value.h"
#include "utils/ctypeutils.h"
#include "utils/methods.h"
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

struct StrFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "str"; }
  Value call(std::vector<Value> args) override {
    return valueToString(args[0]);
  }
};

struct StrSplitFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "strSplit"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string")) {
      throw std::runtime_error("strSplit(): arguments must be strings.");
    }

    std::string base = std::get<std::string>(args[0]);
    std::string delimiter = std::get<std::string>(args[1]);
    return strSplit(base, delimiter);
  }
};
