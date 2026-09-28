#pragma once
#include "ast.h"
#include "environment.h"
#include "interpreter.h"
#include "lang.h"
#include "value.h"

inline void bindFunctionArgs(Interpreter *interp, CallableDecl *decl,
                             std::shared_ptr<Environment> &funcEnv,
                             std::vector<Value> &args,
                             std::unordered_map<std::string, Value> &kwargs) {
  const auto &params = decl->getParams();
  const auto &defaults = decl->getDefaults();

  if (args.size() > params.size())
    throw std::runtime_error(LOC(FN_TOO_MANY_ARGS, decl->getName(),
                                 std::to_string(params.size()),
                                 std::to_string(args.size())));

  for (size_t i = 0; i < params.size(); i++) {
    const std::string &paramName = params[i].lexeme;

    if (i < args.size()) {
      funcEnv->define(paramName, std::move(args[i]));
      continue;
    }

    auto kwIt = kwargs.find(paramName);
    if (kwIt != kwargs.end()) {
      funcEnv->define(paramName, std::move(kwIt->second));
      kwargs.erase(kwIt);
      continue;
    }

    if (defaults[i]) {
      funcEnv->define(paramName, interp->evaluate(defaults[i].get()));
      continue;
    }

    throw std::runtime_error(LOC(FN_MISSING_ARG, decl->getName(), paramName));
  }

  if (!kwargs.empty())
    throw std::runtime_error(
        LOC(FN_UNEXPECTED_KWARG, decl->getName(), kwargs.begin()->first));
}

struct NinFunction : NinCallable {
  CallableDecl *decl;
  std::shared_ptr<Environment> closure;
  Interpreter *interp;
  bool isAsync = false;

  NinFunction(CallableDecl *decl, std::shared_ptr<Environment> closure,
              Interpreter *interp)
      : decl(decl), closure(std::move(closure)), interp(interp) {}

  int arity() override { return (int)decl->getParams().size(); }
  std::string name() override { return decl->getName(); }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
  }

  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    auto funcEnv = std::make_shared<Environment>(closure);
    bindFunctionArgs(interp, decl, funcEnv, args, kwargs);

    try {
      interp->executeBlock(decl->getBody(), std::move(funcEnv));
    } catch (ReturnException &ret) {
      return ret.value;
    }
    return std::monostate{};
  }
};

std::shared_ptr<NinCallable>
makeBoundMethod(std::shared_ptr<NinInstance> inst,
                std::shared_ptr<NinCallable> method);
