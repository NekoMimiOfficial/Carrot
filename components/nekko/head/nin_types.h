#pragma once
#include "ast.h"
#include "environment.h"
#include "interpreter.h"
#include "value.h"

inline void bindFunctionArgs(Interpreter *interp, FunctionStmt *decl,
                             std::shared_ptr<Environment> &funcEnv,
                             std::vector<Value> &args,
                             std::unordered_map<std::string, Value> &kwargs) {
  if (args.size() > decl->params.size())
    throw std::runtime_error("'" + decl->name.lexeme + "' expects at most " +
                             std::to_string(decl->params.size()) +
                             " argument(s) but got " +
                             std::to_string(args.size()) + ".");

  for (size_t i = 0; i < decl->params.size(); i++) {
    const std::string &paramName = decl->params[i].lexeme;

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

    if (decl->defaults[i]) {
      funcEnv->define(paramName, interp->evaluate(decl->defaults[i].get()));
      continue;
    }

    throw std::runtime_error("'" + decl->name.lexeme +
                             "' missing required argument '" + paramName +
                             "'.");
  }

  if (!kwargs.empty())
    throw std::runtime_error("'" + decl->name.lexeme +
                             "' got unexpected keyword argument '" +
                             kwargs.begin()->first + "'.");
}

struct NinFunction : NinCallable {
  FunctionStmt *decl;
  std::shared_ptr<Environment> closure;
  Interpreter *interp;
  bool isAsync = false;

  NinFunction(FunctionStmt *decl, std::shared_ptr<Environment> closure,
              Interpreter *interp)
      : decl(decl), closure(std::move(closure)), interp(interp) {}

  int arity() override { return (int)decl->params.size(); }
  std::string name() override { return decl->name.lexeme; }
  bool isVariadic() override { return true; }

  Value call(std::vector<Value> args) override {
    return callWithKwargs(std::move(args), {});
  }

  Value callWithKwargs(std::vector<Value> args,
                       std::unordered_map<std::string, Value> kwargs) override {
    auto funcEnv = std::make_shared<Environment>(closure);
    bindFunctionArgs(interp, decl, funcEnv, args, kwargs);

    try {
      interp->executeBlock(decl->body, std::move(funcEnv));
    } catch (ReturnException &ret) {
      return ret.value;
    }
    return std::monostate{};
  }
};

std::shared_ptr<NinCallable>
makeBoundMethod(std::shared_ptr<NinInstance> inst,
                std::shared_ptr<NinCallable> method);
