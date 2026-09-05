#include "interpreter.h"
#include "nin_types.h"
#include "value.h"
#include "lang.h"
#include "utils/methods.h"
#include <stdexcept>

void Interpreter::visit(ExprStmt &s) { evaluate(s.expression.get()); }

void Interpreter::visit(VarDecl &s) {
  Value value = std::monostate{};
  if (s.initializer) {
    value = evaluate(s.initializer.get());
  }
  env->define(s.name.lexeme, std::move(value));
}

void Interpreter::visit(ConstDecl &s) {
  Value val = evaluate(s.initializer.get());
  if (std::holds_alternative<std::shared_ptr<NinArray>>(val))
    std::get<std::shared_ptr<NinArray>>(val)->isConst = true;
  env->defineConst(s.name.lexeme, std::move(val));
}

void Interpreter::visit(GlobalDecl &s) {
  Value val = s.initializer ? evaluate(s.initializer.get()) : std::monostate{};
  env->defineGlobal(s.name.lexeme, std::move(val));
}

void Interpreter::visit(MutexDecl &s) {
  Value val = s.initializer ? evaluate(s.initializer.get()) : std::monostate{};
  env->defineMutex(s.name.lexeme, std::move(val));
}

void Interpreter::visit(BlockStmt &s) {
  auto blockEnv = std::make_shared<Environment>(env);
  executeBlock(s.statements, std::move(blockEnv));
}

void Interpreter::visit(IfStmt &s) {
  if (isTruthy(evaluate(s.condition.get()))) {
    execute(s.thenBranch.get());
  } else if (s.elseBranch) {
    execute(s.elseBranch.get());
  }
}

void Interpreter::visit(WhileStmt &s) {
  try {
    while (isTruthy(evaluate(s.condition.get()))) {
      try {
        auto loopEnv = std::make_shared<Environment>(env);
        if (auto *block = dynamic_cast<BlockStmt *>(s.body.get())) {
          executeBlock(block->statements, loopEnv);
        } else {
          auto prevEnv = env;
          env = loopEnv;
          try {
            execute(s.body.get());
          } catch (...) {
            env = prevEnv;
            throw;
          }
          env = prevEnv;
        }
      } catch (ContinueException &) {
      }
    }
  } catch (BreakException &) {
  }
}

void Interpreter::visit(ForStmt &s) {
  auto forEnv = std::make_shared<Environment>(env);
  auto prevEnv = env;
  env = forEnv;

  try {
    if (s.initializer)
      execute(s.initializer.get());

    try {
      while (true) {
        if (s.condition && !isTruthy(evaluate(s.condition.get())))
          break;

        try {
          auto loopEnv = std::make_shared<Environment>(env);
          if (auto *block = dynamic_cast<BlockStmt *>(s.body.get())) {
            executeBlock(block->statements, loopEnv);
          } else {
            auto p = env;
            env = loopEnv;
            try {
              execute(s.body.get());
            } catch (...) {
              env = p;
              throw;
            }
            env = p;
          }
        } catch (ContinueException &) {
        }

        if (s.increment)
          evaluate(s.increment.get());
      }
    } catch (BreakException &) {
    }

  } catch (...) {
    env = prevEnv;
    throw;
  }
  env = prevEnv;
}

void Interpreter::visit(FunctionStmt &s) {
  auto fn = std::make_shared<NinFunction>(&s, env, this);
  env->define(s.name.lexeme, fn);
}

void Interpreter::visit(AsyncFunctionStmt &s) {
  auto fn = std::make_shared<NinFunction>(&s, env, this);
  fn->isAsync = true;
  env->define(s.name.lexeme, fn);
}

void Interpreter::visit(ReturnStmt &s) {
  Value value = std::monostate{};
  if (s.value)
    value = evaluate(s.value.get());
  throw ReturnException(std::move(value));
}

void Interpreter::visit(BreakStmt &s) { throw BreakException{}; }

void Interpreter::visit(ContinueStmt &s) { throw ContinueException{}; }

void Interpreter::visit(FreeStmt &s) { env->free(s.name.lexeme); }

void Interpreter::visit(ClassStmt &s) {
  std::shared_ptr<NinClass> superclass = nullptr;
  if (s.superclass) {
    Value superVal = env->get(s.superclass->lexeme);
    if (!std::holds_alternative<std::shared_ptr<NinClass>>(superVal))
      throw std::runtime_error(LOC(SUPERCLASS_NOT_CLASS, s.superclass->lexeme));
    superclass = std::get<std::shared_ptr<NinClass>>(superVal);
  }

  auto klass = std::make_shared<NinClass>(s.name.lexeme, superclass);
  env->define(s.name.lexeme, klass);

  if (superclass) {
    for (auto &[methodName, method] : superclass->methods) {
      if (methodName == "init")
        continue;
      klass->methods[methodName] = method;
    }
  }

  for (auto &method : s.methods) {
    if (superclass && superclass->methods.count(method->name.lexeme) &&
        method->name.lexeme != "init")
      throw std::runtime_error(LOC(METHOD_NEEDS_OVERRIDE, method->name.lexeme));
    klass->methods[method->name.lexeme] =
        std::make_shared<NinFunction>(method.get(), env, this);
  }

  for (auto &method : s.overrides) {
    if (!superclass || !superclass->methods.count(method->name.lexeme))
      throw std::runtime_error(LOC(OVERRIDE_NOT_IN_PARENT, method->name.lexeme));
    klass->methods[method->name.lexeme] =
        std::make_shared<NinFunction>(method.get(), env, this);
  }
}
