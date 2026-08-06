#pragma once
#include "ast.h"
#include "environment.h"
#include "interpreter.h"
#include "value.h"

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

  Value call(std::vector<Value> args) override {
    auto funcEnv = std::make_shared<Environment>(closure);

    for (int i = 0; i < (int)decl->params.size(); i++) {
      funcEnv->define(decl->params[i].lexeme, args[i]);
    }

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
