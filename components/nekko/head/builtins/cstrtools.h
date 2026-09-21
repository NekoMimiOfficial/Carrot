#pragma once
#include "utils/ctypeutils.h"
#include "utils/methods.h"
#include "value.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

struct StrFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "to_string"; }
  Value call(std::vector<Value> args) override {
    return valueToString(args[0]);
  }
};

struct StrSplitFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "split"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string")) {
      throw std::runtime_error("split(): arguments must be strings.");
    }

    std::string base = std::get<std::string>(args[0]);
    std::string delimiter = std::get<std::string>(args[1]);
    return strSplit(base, delimiter);
  }
};

struct StrFindFn : NinCallable {
  int arity() override { return 3; }
  std::string name() override { return "find"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string"))
      throw std::runtime_error("find(): First two arguments must be a string.");

    if (!checkArgs(args[2], "number"))
      throw std::runtime_error("find(): Thrid argument must be a number.");

    std::string baseStr = std::get<std::string>(args[0]);
    std::string denom = std::get<std::string>(args[1]);
    double indx = std::get<double>(args[2]);

    if (!isInt(indx))
      throw std::runtime_error(
          "find(): Third argument must be an int, not a double");
    int fidx = static_cast<int>(indx);
    size_t pos;
    if (fidx < 0) {
      pos = baseStr.find(denom);
    } else {
      pos = baseStr.find(denom, fidx);
    }
    if (pos != std::string::npos) {
      return static_cast<double>(pos);
    } else {
      return -1.0;
    }
  }
};

struct StrReplaceFn : NinCallable {
  int arity() override { return 3; }
  std::string name() override { return "replace"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string") ||
        !checkArgs(args[2], "string"))
      throw std::runtime_error(
          "replace(): excpects 3 arguments of type 'string'.");

    std::string base = std::get<std::string>(args[0]);
    std::string old = std::get<std::string>(args[1]);
    std::string new_w = std::get<std::string>(args[2]);

    strReplace(base, old, new_w);

    return base;
  }
};

struct StrJoinFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "join"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "array"))
      throw std::runtime_error("join(): first argument must be an array.");

    if (!checkArgs(args[1], "string"))
      throw std::runtime_error("join(): second argument must be a string.");

    return strJoin(std::get<std::shared_ptr<NinArray>>(args[0]),
                   std::get<std::string>(args[1]));
  }
};

struct StrStartswithFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "startswith"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string"))
      throw std::runtime_error("startswith(): expects 2 string arguments.");

    return std::get<std::string>(args[0]).rfind(std::get<std::string>(args[1]),
                                                0) == 0;
  }
};

struct StrEndswithFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "endswith"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string"))
      throw std::runtime_error("endswith(): expects 2 string arguments.");

    std::string str = std::get<std::string>(args[0]);
    std::string suffix = std::get<std::string>(args[1]);

    return (str.length() >= suffix.length())
               ? (0 == str.compare(str.length() - suffix.length(),
                                   suffix.length(), suffix))
               : false;
  }
};

struct StrRmprefixFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "rmprefix"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string"))
      throw std::runtime_error("rmprefix(): expects 2 string arguments.");

    return (std::get<std::string>(args[0]).rfind(std::get<std::string>(args[1]),
                                                 0) == 0)
               ? std::get<std::string>(args[0]).substr(
                     std::get<std::string>(args[1]).length())
               : std::get<std::string>(args[0]);
  }
};

struct StrRmsuffixFn : NinCallable {
  int arity() override { return 2; }
  std::string name() override { return "rmsuffix"; }
  Value call(std::vector<Value> args) override {
    if (!checkArgs(args[0], "string") || !checkArgs(args[1], "string"))
      throw std::runtime_error("rmsuffix(): expects 2 string arguments.");

    std::string str = std::get<std::string>(args[0]);
    const std::string &suffix = std::get<std::string>(args[1]);

    if (str.length() >= suffix.length() &&
        str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
            0) {
      str.erase(str.length() - suffix.length());
    }

    return str;
  }
};

std::shared_ptr<NinInstance> StringTools() {
  std::shared_ptr<NinClass> klass = std::make_shared<NinClass>("__builtin_cl_01");
  std::shared_ptr<NinInstance> inst = std::make_shared<NinInstance>(klass);

  inst->fields["to_string"] = std::make_shared<StrFn>();
  inst->fields["split"] = std::make_shared<StrSplitFn>();
  inst->fields["join"] = std::make_shared<StrJoinFn>();
  inst->fields["find"] = std::make_shared<StrFindFn>();
  inst->fields["replace"] = std::make_shared<StrReplaceFn>();
  inst->fields["startswith"] = std::make_shared<StrStartswithFn>();
  inst->fields["endswith"] = std::make_shared<StrEndswithFn>();
  inst->fields["rmprefix"] = std::make_shared<StrRmprefixFn>();
  inst->fields["rmsuffix"] = std::make_shared<StrRmsuffixFn>();

  return inst;
}
