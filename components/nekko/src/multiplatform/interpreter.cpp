#include "interpreter.h"
#include "builtin.h"
#include "coroutine.h"
#include "nin_types.h"
#include "platform.h"
#include "value.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

extern thread_local bool insideCoroutine;
thread_local std::shared_ptr<Environment> Interpreter::env;

void Interpreter::reset(std::string gotSourceDir,
                        std::vector<std::string> argvIn) {
  globals = std::make_shared<Environment>();
  env = globals;
  sourceDir = gotSourceDir;

  auto reg = [&](std::shared_ptr<NinCallable> fn) {
    globals->define(fn->name(), fn);
  };

  std::vector<Value> argvValues;
  for (auto &a : argvIn)
    argvValues.push_back(a);
  auto argvArray = std::make_shared<NinArray>(std::move(argvValues));

  reg(std::make_shared<ImportFn>(this, std::move(sourceDir)));

  reg(std::make_shared<InputFn>());
  reg(std::make_shared<SystemFn>());
  reg(std::make_shared<ClockFn>());
  reg(std::make_shared<ArgvFn>(argvArray));
  reg(std::make_shared<ExitFn>());
  reg(std::make_shared<SleepFn>());

  reg(std::make_shared<StrFn>());
  reg(std::make_shared<NumFn>());
  reg(std::make_shared<TypeFn>());
  reg(std::make_shared<LenFn>());
  reg(std::make_shared<PushFn>());
  reg(std::make_shared<PopFn>());

  registerHandler(this);
  registerPlatformHandler(this);
}

Interpreter::Interpreter(std::string sourceDir,
                         std::vector<std::string> argvIn) {
  reset(std::move(sourceDir), std::move(argvIn));
}

void Interpreter::interpret(const std::vector<StmtPtr> &statements) {
  try {
    for (const auto &stmt : statements) {
      execute(stmt.get());
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "\n[Runtime Error] " << e.what() << "\n";
  }
}

void Interpreter::executeBlock(const std::vector<StmtPtr> &stmts,
                               std::shared_ptr<Environment> blockEnv) {
  auto previous = env;
  env = std::move(blockEnv);
  try {
    for (const auto &stmt : stmts) {
      execute(stmt.get());
    }
  } catch (...) {
    env = previous;
    throw;
  }
  env = previous;
}

std::shared_ptr<NinCallable>
makeBoundMethod(std::shared_ptr<NinInstance> inst,
                std::shared_ptr<NinCallable> method) {
  struct BoundMethod : NinCallable {
    std::shared_ptr<NinInstance> inst;
    std::shared_ptr<NinCallable> inner;

    BoundMethod(std::shared_ptr<NinInstance> i, std::shared_ptr<NinCallable> m)
        : inst(std::move(i)), inner(std::move(m)) {}

    int arity() override { return inner->arity(); }
    std::string name() override { return inner->name(); }

    Value call(std::vector<Value> args) override {
      auto *fn = dynamic_cast<NinFunction *>(inner.get());
      if (!fn)
        return inner->call(std::move(args));

      auto thisEnv = std::make_shared<Environment>(fn->closure);
      thisEnv->define("this", inst);

      NinFunction bound(fn->decl, thisEnv, fn->interp);
      return bound.call(std::move(args));
    }
  };
  return std::make_shared<BoundMethod>(std::move(inst), std::move(method));
}

void Interpreter::checkNumberOperand(const Token &op, const Value &val) {
  if (std::holds_alternative<double>(val))
    return;
  throw std::runtime_error("Operand of '" + op.lexeme +
                           "' must be a number. (line " +
                           std::to_string(op.line) + ")");
}

void Interpreter::checkNumberOperands(const Token &op, const Value &left,
                                      const Value &right) {
  if (std::holds_alternative<double>(left) &&
      std::holds_alternative<double>(right))
    return;
  throw std::runtime_error("Both operands of '" + op.lexeme +
                           "' must be numbers. (line " +
                           std::to_string(op.line) + ")");
}

Value Interpreter::getCachedModule(const std::string &key) {
  std::lock_guard<std::mutex> lock(moduleCacheMutex);
  auto it = moduleCache.find(key);
  if (it != moduleCache.end())
    return it->second;
  return std::monostate{};
}

void Interpreter::cacheModule(const std::string &key, Value mod) {
  std::lock_guard<std::mutex> lock(moduleCacheMutex);
  moduleCache[key] = std::move(mod);
}
