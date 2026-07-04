#include "interpreter.h"
#include "builtin.h"
#include "coroutine.h"
#include "platform.h"
#include "value.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

extern thread_local bool insideCoroutine;

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

void Interpreter::reset(std::string sourceDir,
                        std::vector<std::string> argvIn) {
  globals = std::make_shared<Environment>();
  env = globals;

  auto reg = [&](std::shared_ptr<NinCallable> fn) {
    globals->define(fn->name(), fn);
  };

  std::vector<Value> argvValues;
  for (auto &a : argvIn)
    argvValues.push_back(a);
  auto argvArray = std::make_shared<NinArray>(std::move(argvValues));

  // builtin registration is done here
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

  // external platform callable loader
  registerPlatformBuiltins(this);
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

void Interpreter::execute(Stmt *stmt) {

  if (auto *s = dynamic_cast<ExprStmt *>(stmt)) {
    evaluate(s->expression.get());

  } else if (auto *s = dynamic_cast<PrintStmt *>(stmt)) {
    Value val = evaluate(s->expression.get());
    std::cout << valueToString(val) << "\n";

  } else if (auto *s = dynamic_cast<VarDecl *>(stmt)) {
    Value value = std::monostate{};
    if (s->initializer) {
      value = evaluate(s->initializer.get());
    }
    env->define(s->name.lexeme, std::move(value));

  } else if (auto *s = dynamic_cast<GlobalDecl *>(stmt)) {
    Value val =
        s->initializer ? evaluate(s->initializer.get()) : std::monostate{};
    env->defineGlobal(s->name.lexeme, std::move(val));
  } else if (auto *s = dynamic_cast<ConstDecl *>(stmt)) {
    Value val = evaluate(s->initializer.get());
    if (std::holds_alternative<std::shared_ptr<NinArray>>(val))
      std::get<std::shared_ptr<NinArray>>(val)->isConst = true;
    env->defineConst(s->name.lexeme, std::move(val));
  } else if (auto *s = dynamic_cast<BlockStmt *>(stmt)) {
    auto blockEnv = std::make_shared<Environment>(env);
    executeBlock(s->statements, std::move(blockEnv));

  } else if (auto *s = dynamic_cast<IfStmt *>(stmt)) {
    if (isTruthy(evaluate(s->condition.get()))) {
      execute(s->thenBranch.get());
    } else if (s->elseBranch) {
      execute(s->elseBranch.get());
    }

  } else if (auto *s = dynamic_cast<WhileStmt *>(stmt)) {
    try {
      while (isTruthy(evaluate(s->condition.get()))) {
        try {
          auto loopEnv = std::make_shared<Environment>(env);
          executeBlock(dynamic_cast<BlockStmt *>(s->body.get())->statements,
                       loopEnv);
        } catch (ContinueException &) {
        }
      }
    } catch (BreakException &) {
    }

  } else if (auto *s = dynamic_cast<ForStmt *>(stmt)) {
    auto forEnv = std::make_shared<Environment>(env);
    auto prevEnv = env;
    env = forEnv;

    try {
      if (s->initializer)
        execute(s->initializer.get());

      try {
        while (true) {
          if (s->condition && !isTruthy(evaluate(s->condition.get())))
            break;

          try {
            auto loopEnv = std::make_shared<Environment>(env);
            executeBlock(dynamic_cast<BlockStmt *>(s->body.get())->statements,
                         loopEnv);
          } catch (ContinueException &) {
          }

          if (s->increment)
            evaluate(s->increment.get());
        }
      } catch (BreakException &) {
      }

    } catch (...) {
      env = prevEnv;
      throw;
    }
    env = prevEnv;

  } else if (auto *s = dynamic_cast<AsyncFunctionStmt *>(stmt)) {
    auto fn = std::make_shared<NinFunction>(s, env, this);
    fn->isAsync = true;
    env->define(s->name.lexeme, fn);

  } else if (auto *s = dynamic_cast<FunctionStmt *>(stmt)) {

    auto fn = std::make_shared<NinFunction>(s, env, this);
    env->define(s->name.lexeme, fn);

  } else if (auto *s = dynamic_cast<ReturnStmt *>(stmt)) {
    Value value = std::monostate{};
    if (s->value)
      value = evaluate(s->value.get());
    throw ReturnException(std::move(value));

  } else if (dynamic_cast<BreakStmt *>(stmt)) {
    throw BreakException{};

  } else if (auto *s = dynamic_cast<ClassStmt *>(stmt)) {
    std::shared_ptr<NinClass> superclass = nullptr;
    if (s->superclass) {
      Value superVal = env->get(s->superclass->lexeme);
      if (!std::holds_alternative<std::shared_ptr<NinClass>>(superVal))
        throw std::runtime_error("'" + s->superclass->lexeme +
                                 "' is not a class.");
      superclass = std::get<std::shared_ptr<NinClass>>(superVal);
    }

    auto klass = std::make_shared<NinClass>(s->name.lexeme, superclass);
    env->define(s->name.lexeme, klass);

    if (superclass) {
      for (auto &[methodName, method] : superclass->methods) {
        if (methodName == "init")
          continue;
        klass->methods[methodName] = method;
      }
    }

    for (auto &method : s->methods) {
      if (superclass && superclass->methods.count(method->name.lexeme) &&
          method->name.lexeme != "init")
        throw std::runtime_error(
            "Method '" + method->name.lexeme +
            "' exists in parent class. Use 'override' to replace it.");
      klass->methods[method->name.lexeme] =
          std::make_shared<NinFunction>(method.get(), env, this);
    }

    for (auto &method : s->overrides) {
      if (!superclass || !superclass->methods.count(method->name.lexeme))
        throw std::runtime_error("Cannot override '" + method->name.lexeme +
                                 "': not defined in parent class.");
      klass->methods[method->name.lexeme] =
          std::make_shared<NinFunction>(method.get(), env, this);
    }
  } else if (auto *s = dynamic_cast<FreeStmt *>(stmt)) {
    env->free(s->name.lexeme);
  } else if (dynamic_cast<ContinueStmt *>(stmt)) {
    throw ContinueException{};
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

static std::shared_ptr<NinCallable>
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

Value Interpreter::evaluate(Expr *expr) {
  if (auto *e = dynamic_cast<LiteralExpr *>(expr)) {
    if (std::holds_alternative<double>(e->value))
      return std::get<double>(e->value);
    if (std::holds_alternative<std::string>(e->value))
      return std::get<std::string>(e->value);
    if (std::holds_alternative<bool>(e->value))
      return std::get<bool>(e->value);
    return std::monostate{};

  } else if (auto *e = dynamic_cast<VariableExpr *>(expr)) {
    return env->get(e->name.lexeme);

  } else if (auto *e = dynamic_cast<AssignExpr *>(expr)) {
    Value val = evaluate(e->value.get());
    env->assign(e->name.lexeme, val);
    return val;

  } else if (auto *e = dynamic_cast<UnaryExpr *>(expr)) {
    Value right = evaluate(e->right.get());

    switch (e->op.type) {
    case TokenType::MINUS:
      checkNumberOperand(e->op, right);
      return -std::get<double>(right);
    case TokenType::BANG:
      return !isTruthy(right);
    default:
      break;
    }

  } else if (auto *e = dynamic_cast<LogicalExpr *>(expr)) {
    Value left = evaluate(e->left.get());

    if (e->op.type == TokenType::OR) {
      if (isTruthy(left))
        return left;
    } else {
      if (!isTruthy(left))
        return left;
    }
    return evaluate(e->right.get());

  } else if (auto *e = dynamic_cast<BinaryExpr *>(expr)) {
    Value left = evaluate(e->left.get());
    Value right = evaluate(e->right.get());

    switch (e->op.type) {
    case TokenType::PLUS:
      if (std::holds_alternative<double>(left) &&
          std::holds_alternative<double>(right))
        return std::get<double>(left) + std::get<double>(right);

      if (std::holds_alternative<std::string>(left) ||
          std::holds_alternative<std::string>(right))
        return valueToString(left) + valueToString(right);
      throw std::runtime_error(
          "Operands of '+' must be numbers or strings. (line " +
          std::to_string(e->op.line) + ")");
    case TokenType::MINUS:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) - std::get<double>(right);
    case TokenType::STAR:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) * std::get<double>(right);
    case TokenType::SLASH:
      checkNumberOperands(e->op, left, right);
      if (std::get<double>(right) == 0.0)
        throw std::runtime_error("Division by zero. (line " +
                                 std::to_string(e->op.line) + ")");
      return std::get<double>(left) / std::get<double>(right);
    case TokenType::PERCENT:
      checkNumberOperands(e->op, left, right);
      return std::fmod(std::get<double>(left), std::get<double>(right));
    case TokenType::GREATER:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESS:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESS_EQUAL:
      checkNumberOperands(e->op, left, right);
      return std::get<double>(left) <= std::get<double>(right);
    case TokenType::EQUAL_EQUAL:
      return isEqual(left, right);
    case TokenType::BANG_EQUAL:
      return !isEqual(left, right);
    default:
      break;
    }

  } else if (auto *e = dynamic_cast<CallExpr *>(expr)) {
    Value callee = evaluate(e->callee.get());

    std::vector<Value> args;
    for (const auto &arg : e->arguments) {
      args.push_back(evaluate(arg.get()));
    }

    if (!std::holds_alternative<std::shared_ptr<NinCallable>>(callee)) {
      throw std::runtime_error("Can only call functions. (line " +
                               std::to_string(e->paren.line) + ")");
    }

    auto fn = std::get<std::shared_ptr<NinCallable>>(callee);

    if ((int)args.size() != fn->arity()) {
      throw std::runtime_error(
          "'" + fn->name() + "' expects " + std::to_string(fn->arity()) +
          " argument(s) but got " + std::to_string(args.size()) + ". (line " +
          std::to_string(e->paren.line) + ")");
    }

    return fn->call(std::move(args));

  } else if (auto *e = dynamic_cast<ArrayExpr *>(expr)) {
    std::vector<Value> elements;
    for (const auto &elem : e->elements) {
      elements.push_back(evaluate(elem.get()));
    }
    return std::make_shared<NinArray>(std::move(elements));

  } else if (auto *e = dynamic_cast<IndexExpr *>(expr)) {
    Value obj = evaluate(e->object.get());
    Value idx = evaluate(e->index.get());

    if (std::holds_alternative<std::shared_ptr<NinArray>>(obj)) {
      auto arr = std::get<std::shared_ptr<NinArray>>(obj);
      if (!std::holds_alternative<double>(idx))
        throw std::runtime_error("Array index must be a number.");
      int i = (int)std::get<double>(idx);
      if (i < 0)
        i = (int)arr->elements.size() + i;
      if (i < 0 || i >= (int)arr->elements.size())
        throw std::runtime_error("Array index " + std::to_string(i) +
                                 " out of bounds.");
      return arr->elements[i];
    }

    if (std::holds_alternative<std::string>(obj)) {
      const std::string &s = std::get<std::string>(obj);
      if (!std::holds_alternative<double>(idx))
        throw std::runtime_error("String index must be a number.");
      int i = (int)std::get<double>(idx);
      if (i < 0)
        i = (int)s.size() + i;
      if (i < 0 || i >= (int)s.size())
        throw std::runtime_error("String index " + std::to_string(i) +
                                 " out of bounds.");
      return std::string(1, s[i]);
    }

    throw std::runtime_error("Cannot index into this type. (line " +
                             std::to_string(e->bracket.line) + ")");

  } else if (auto *e = dynamic_cast<IndexAssignExpr *>(expr)) {
    Value obj = evaluate(e->object.get());
    Value idx = evaluate(e->index.get());
    Value val = evaluate(e->value.get());

    if (auto *v = dynamic_cast<VariableExpr *>(e->object.get())) {
      if (env->isConst(v->name.lexeme))
        throw std::runtime_error("Cannot modify const array '" +
                                 v->name.lexeme + "'.");
    }

    if (!std::holds_alternative<std::shared_ptr<NinArray>>(obj))
      throw std::runtime_error("Can only index-assign into arrays.");
    auto arr = std::get<std::shared_ptr<NinArray>>(obj);
    if (arr->isConst)
      throw std::runtime_error("Cannot modify a const array.");
    if (!std::holds_alternative<double>(idx))
      throw std::runtime_error("Array index must be a number.");
    int i = (int)std::get<double>(idx);
    if (i < 0)
      i = (int)arr->elements.size() + i;
    if (i < 0 || i >= (int)arr->elements.size())
      throw std::runtime_error("Array index " + std::to_string(i) +
                               " out of bounds.");
    arr->elements[i] = val;
    return val;
  } else if (auto *e = dynamic_cast<GetExpr *>(expr)) {
    Value obj = evaluate(e->object.get());

    if (std::holds_alternative<std::shared_ptr<NinNative>>(obj)) {
      auto native = std::get<std::shared_ptr<NinNative>>(obj);
      return native->getField(e->name.lexeme);
    }

    if (std::holds_alternative<std::shared_ptr<NinModule>>(obj)) {
      auto mod = std::get<std::shared_ptr<NinModule>>(obj);
      auto it = mod->members.find(e->name.lexeme);
      if (it != mod->members.end())
        return it->second;
      throw std::runtime_error("Module has no member '" + e->name.lexeme +
                               "'.");
    }

    if (std::holds_alternative<std::shared_ptr<NinInstance>>(obj)) {
      auto inst = std::get<std::shared_ptr<NinInstance>>(obj);
      auto it = inst->fields.find(e->name.lexeme);
      if (it != inst->fields.end())
        return it->second;
      auto mit = inst->klass->methods.find(e->name.lexeme);
      if (mit != inst->klass->methods.end())
        return makeBoundMethod(inst, mit->second);
      throw std::runtime_error("Undefined property '" + e->name.lexeme + "'.");
    }

    throw std::runtime_error(
        "Only modules and instances have properties. (line " +
        std::to_string(e->name.line) + ")");

  } else if (auto *e = dynamic_cast<SetExpr *>(expr)) {
    Value obj = evaluate(e->object.get());
    Value val = evaluate(e->value.get());

    if (std::holds_alternative<std::shared_ptr<NinNative>>(obj)) {
      auto native = std::get<std::shared_ptr<NinNative>>(obj);
      native->setField(e->name.lexeme, val);
      return val;
    }

    if (std::holds_alternative<std::shared_ptr<NinModule>>(obj)) {
      auto mod = std::get<std::shared_ptr<NinModule>>(obj);
      mod->members[e->name.lexeme] = val;
      return val;
    }

    if (std::holds_alternative<std::shared_ptr<NinInstance>>(obj)) {
      auto inst = std::get<std::shared_ptr<NinInstance>>(obj);
      inst->fields[e->name.lexeme] = val;
      return val;
    }

    throw std::runtime_error(
        "Only modules and instances have settable properties. (line " +
        std::to_string(e->name.line) + ")");

  } else if (auto *e = dynamic_cast<NewExpr *>(expr)) {
    Value classVal = evaluate(e->classExpr.get());
    if (!std::holds_alternative<std::shared_ptr<NinClass>>(classVal))
      throw std::runtime_error("Expression after 'new' is not a class. (line " +
                               std::to_string(e->keyword.line) + ")");

    auto klass = std::get<std::shared_ptr<NinClass>>(classVal);
    auto inst = std::make_shared<NinInstance>(klass);

    std::vector<Value> args;
    for (auto &arg : e->arguments)
      args.push_back(evaluate(arg.get()));

    auto it = klass->methods.find("init");
    if (it != klass->methods.end()) {
      auto bound = makeBoundMethod(inst, it->second);
      if ((int)args.size() != bound->arity())
        throw std::runtime_error("'" + klass->className + ".init' expects " +
                                 std::to_string(bound->arity()) +
                                 " argument(s) but got " +
                                 std::to_string(args.size()) + ".");
      bound->call(std::move(args));
    } else if (!args.empty()) {
      throw std::runtime_error(
          "Class '" + klass->className +
          "' has no 'init' but was called with arguments.");
    }

    return inst;

  } else if (auto *e = dynamic_cast<ThisExpr *>(expr)) {
    return env->get("this");
  } else if (auto *e = dynamic_cast<CoroutineExpr *>(expr)) {
    Value fnVal = env->get(e->fnName.lexeme);
    if (!std::holds_alternative<std::shared_ptr<NinCallable>>(fnVal))
      throw std::runtime_error("coroutine: '" + e->fnName.lexeme +
                               "' is not a function.");

    auto fn = std::get<std::shared_ptr<NinCallable>>(fnVal);
    auto *ninFn = dynamic_cast<NinFunction *>(fn.get());
    if (!ninFn || !ninFn->isAsync)
      throw std::runtime_error("coroutine: '" + e->fnName.lexeme +
                               "' is not an async function.");

    std::vector<Value> args;
    for (auto &arg : e->arguments)
      args.push_back(evaluate(arg.get()));

    auto capturedClosure = ninFn->closure;
    auto capturedDecl = ninFn->decl;

    auto coro = std::make_shared<NinCoroutine>(
        [this, capturedDecl, capturedClosure, args]() mutable -> Value {
          auto funcEnv = std::make_shared<Environment>(capturedClosure);
          for (int i = 0; i < (int)capturedDecl->params.size(); i++)
            funcEnv->define(capturedDecl->params[i].lexeme, args[i]);

          try {
            executeBlock(capturedDecl->body, funcEnv);
          } catch (ReturnException &ret) {
            return ret.value;
          }
          return std::monostate{};
        });

    auto klass = std::make_shared<NinClass>("coroutine");
    auto inst = std::make_shared<NinInstance>(klass);

    struct RunFn : NinCallable {
      std::shared_ptr<NinCoroutine> coro;
      explicit RunFn(std::shared_ptr<NinCoroutine> c) : coro(std::move(c)) {}
      int arity() override { return 0; }
      std::string name() override { return "run"; }
      Value call(std::vector<Value>) override {
        coroutineRun(coro);
        return std::monostate{};
      }
    };

    struct YieldFn : NinCallable {
      std::shared_ptr<NinCoroutine> coro;
      explicit YieldFn(std::shared_ptr<NinCoroutine> c) : coro(std::move(c)) {}
      int arity() override { return 1; }
      std::string name() override { return "yield"; }
      Value call(std::vector<Value> args) override {
        if (coro->state == NinCoroutine::State::DONE) {
          coroutineJoin(coro);
          return coro->getReturn();
        }
        return args[0];
      }
    };

    inst->fields["run"] = std::make_shared<RunFn>(coro);
    inst->fields["yield"] = std::make_shared<YieldFn>(coro);
    return inst;

  } else if (auto *e = dynamic_cast<SuperExpr *>(expr)) {
    Value thisVal = env->get("this");
    if (!std::holds_alternative<std::shared_ptr<NinInstance>>(thisVal))
      throw std::runtime_error("'super' used outside a class method.");

    auto inst = std::get<std::shared_ptr<NinInstance>>(thisVal);
    auto superclass = inst->klass->superclass;
    if (!superclass)
      throw std::runtime_error("Class '" + inst->klass->className +
                               "' has no parent class.");

    auto it = superclass->methods.find("init");
    if (it == superclass->methods.end())
      throw std::runtime_error("Parent class has no 'init' method.");

    std::vector<Value> args;
    for (auto &arg : e->arguments)
      args.push_back(evaluate(arg.get()));

    auto bound = makeBoundMethod(inst, it->second);
    if ((int)args.size() != bound->arity())
      throw std::runtime_error(
          "super() expects " + std::to_string(bound->arity()) +
          " argument(s) but got " + std::to_string(args.size()) + ".");
    bound->call(std::move(args));
    return std::monostate{};
  } else if (auto *e = dynamic_cast<AwaitExpr *>(expr)) {
    if (!insideCoroutine)
      throw std::runtime_error(
          "'await' used outside a running coroutine. (line " +
          std::to_string(e->keyword.line) + ")");
    return evaluate(e->value.get());
  }

  return std::monostate{};
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
