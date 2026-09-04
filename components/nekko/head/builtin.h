#pragma once
#include "interpreter.h"
#include "lexer.h"
#include "methods.h"
#include "parser.h"
#include "value.h"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>

#include "platform_core_builtins.h"

struct ClockFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "clock"; }
  Value call(std::vector<Value>) override {
    using namespace std::chrono;
    auto t = system_clock::now().time_since_epoch();
    return (double)duration_cast<milliseconds>(t).count() / 1000.0;
  }
};

struct StrFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "str"; }
  Value call(std::vector<Value> args) override {
    return valueToString(args[0]);
  }
};

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

struct PushFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "push"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::shared_ptr<NinArray>>(args[0]))
      throw std::runtime_error("push(): first argument must be an array.");
    if (std::get<std::shared_ptr<NinArray>>(args[0])->isConst)
      throw std::runtime_error("push(): cannot modify a const array.");
    std::get<std::shared_ptr<NinArray>>(args[0])->elements.push_back(args[1]);
    return args[0];
  }
};

struct PopFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "pop"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::shared_ptr<NinArray>>(args[0]))
      throw std::runtime_error("pop(): argument must be an array.");
    auto arr = std::get<std::shared_ptr<NinArray>>(args[0]);
    if (arr->isConst)
      throw std::runtime_error("pop(): cannot modify a const array.");
    if (arr->elements.empty())
      throw std::runtime_error("pop(): array is empty.");
    Value last = arr->elements.back();
    arr->elements.pop_back();
    return last;
  }
};

struct SystemFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "system"; }
  Value call(std::vector<Value> args) override {
    if (!(std::holds_alternative<std::string>(args[0]))) {
      throw std::runtime_error("system(): argument must be a string.");
    }
    return "[exit code: " +
           std::to_string(system(valueToString(args[0]).c_str())) + "]";
  }
};

struct ImportFn : NinCallable {
  Interpreter *interp;
  std::string callerDir;

  explicit ImportFn(Interpreter *interp, std::string callerDir)
      : interp(interp), callerDir(std::move(callerDir)) {}

  int arity() override { return 1; }
  std::string name() override { return "import"; }

  Value call(std::vector<Value> args) override;
};

struct ArgvFn : NinCallable {
  std::shared_ptr<NinArray> args;
  explicit ArgvFn(std::shared_ptr<NinArray> args) : args(std::move(args)) {}

  int arity() override { return 0; }
  std::string name() override { return "argv"; }
  Value call(std::vector<Value>) override { return args; }
};

struct ExitFn : NinCallable {
  int arity() override { return 0; }
  bool isVariadic() override { return true; }
  std::string name() override { return "exit"; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
  }

  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override;
};

struct SleepFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "sleep"; }
  Value call(std::vector<Value> args) override;
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
