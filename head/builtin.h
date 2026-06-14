#pragma once
#include "interpreter.h"
#include "lexer.h"
#include "networking.h"
#include "parser.h"
#include "value.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>

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
    const Value &v = args[0];
    if (std::holds_alternative<std::monostate>(v))
      return std::string("nil");
    if (std::holds_alternative<double>(v))
      return std::string("number");
    if (std::holds_alternative<std::string>(v))
      return std::string("string");
    if (std::holds_alternative<bool>(v))
      return std::string("bool");
    if (std::holds_alternative<std::shared_ptr<NinCallable>>(v))
      return std::string("function");
    if (std::holds_alternative<std::shared_ptr<NinArray>>(v))
      return std::string("array");
    if (std::holds_alternative<std::shared_ptr<NinModule>>(v))
      return std::string("module");
    if (std::holds_alternative<std::shared_ptr<NinClass>>(v))
      return std::string("class");
    if (std::holds_alternative<std::shared_ptr<NinInstance>>(v))
      return std::string("instance");
    return std::string("unknown");
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
    if (arr->elements.empty())
      throw std::runtime_error("pop(): array is empty.");
    Value last = arr->elements.back();
    arr->elements.pop_back();
    return last;
  }
};

struct SqrtFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "sqrt"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]))
      throw std::runtime_error("sqrt(): argument must be a number.");
    return std::sqrt(std::get<double>(args[0]));
  }
};

struct AbsFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "abs"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]))
      throw std::runtime_error("abs(): argument must be a number.");
    return std::abs(std::get<double>(args[0]));
  }
};

struct FloorFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "floor"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]))
      throw std::runtime_error("floor(): argument must be a number.");
    return std::floor(std::get<double>(args[0]));
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

struct GetFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "get"; }
  Value call(std::vector<Value> args) override {
    if (!(std::holds_alternative<std::string>(args[0]))) {
      throw std::runtime_error("get(): argument must be a string.");
    }
    return doGetRequest(valueToString(args[0]));
  }
};

struct ImportFn : NinCallable {
  Interpreter *interp;
  std::string callerDir;

  ImportFn(Interpreter *interp, std::string callerDir)
      : interp(interp), callerDir(std::move(callerDir)) {}

  int arity() override { return 1; }
  std::string name() override { return "import"; }

  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("import(): argument must be a string path.");

    std::string rel = std::get<std::string>(args[0]);
    std::filesystem::path path = std::filesystem::path(callerDir) / rel;

    std::ifstream file(path);
    if (!file.is_open())
      throw std::runtime_error("import(): cannot open '" + path.string() +
                               "'.");

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    Lexer modLex(source);
    auto tokens = modLex.tokenize();
    Parser modParser(std::move(tokens));
    auto stmts = modParser.parse();

    auto mod = std::make_shared<NinModule>();
    mod->sourcePath = path.string();
    mod->ast = std::move(stmts);

    auto modEnv = std::make_shared<Environment>(interp->globals);
    interp->executeBlock(mod->ast, modEnv);
    mod->members = modEnv->exportAll();
    return mod;
  }
};
