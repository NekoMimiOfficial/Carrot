#include "coroutine.h"
#include "interpreter.h"
#include "nin_types.h"
#include "value.h"
#include "utils/methods.h"
#include "lang.h"
#include <cmath>
#include <cstdint>
#include <stdexcept>

extern thread_local bool insideCoroutine;

Value Interpreter::visit(LiteralExpr &e) {
  if (std::holds_alternative<double>(e.value))
    return std::get<double>(e.value);
  if (std::holds_alternative<std::string>(e.value))
    return std::get<std::string>(e.value);
  if (std::holds_alternative<bool>(e.value))
    return std::get<bool>(e.value);
  if (std::holds_alternative<uint8_t>(e.value))
    return std::get<uint8_t>(e.value);
  return std::monostate{};
}

Value Interpreter::visit(VariableExpr &e) { return env->get(e.name.lexeme); }

Value Interpreter::visit(AssignExpr &e) {
  Value val = evaluate(e.value.get());
  env->assign(e.name.lexeme, val);
  return val;
}

Value Interpreter::visit(UnaryExpr &e) {
  Value right = evaluate(e.right.get());

  switch (e.op.type) {
  case TokenType::MINUS:
    checkNumberOperand(e.op, right);
    return -std::get<double>(right);
  case TokenType::BANG:
    return !isTruthy(right);
  default:
    break;
  }
  return std::monostate{};
}

Value Interpreter::visit(LogicalExpr &e) {
  Value left = evaluate(e.left.get());

  if (e.op.type == TokenType::OR) {
    if (isTruthy(left))
      return left;
  } else {
    if (!isTruthy(left))
      return left;
  }
  return evaluate(e.right.get());
}

Value Interpreter::visit(BinaryExpr &e) {
  Value left = evaluate(e.left.get());
  Value right = evaluate(e.right.get());

  switch (e.op.type) {
  case TokenType::PLUS:
    if (std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right))
      return std::get<double>(left) + std::get<double>(right);

    if (std::holds_alternative<std::string>(left) ||
        std::holds_alternative<std::string>(right))
      return valueToString(left) + valueToString(right);
    throw std::runtime_error(LOC(OP_PLUS_MISMATCH, std::to_string(e.op.line)));
  case TokenType::MINUS:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) - std::get<double>(right);
  case TokenType::STAR:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) * std::get<double>(right);
  case TokenType::SLASH:
    checkNumberOperands(e.op, left, right);
    if (std::get<double>(right) == 0.0)
      throw std::runtime_error(LOC(DIV_ZERO, std::to_string(e.op.line)));
    return std::get<double>(left) / std::get<double>(right);
  case TokenType::PERCENT:
    checkNumberOperands(e.op, left, right);
    return std::fmod(std::get<double>(left), std::get<double>(right));
  case TokenType::GREATER:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) > std::get<double>(right);
  case TokenType::GREATER_EQUAL:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) >= std::get<double>(right);
  case TokenType::LESS:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) < std::get<double>(right);
  case TokenType::LESS_EQUAL:
    checkNumberOperands(e.op, left, right);
    return std::get<double>(left) <= std::get<double>(right);
  case TokenType::EQUAL_EQUAL:
    return isEqual(left, right);
  case TokenType::BANG_EQUAL:
    return !isEqual(left, right);
  default:
    break;
  }
  return std::monostate{};
}

Value Interpreter::visit(CallExpr &e) {
  Value callee = evaluate(e.callee.get());

  std::vector<Value> args;
  for (const auto &arg : e.arguments)
    args.push_back(evaluate(arg.get()));

  std::unordered_map<std::string, Value> kwargs;
  for (const auto &[kwName, kwExpr] : e.kwargs)
    kwargs[kwName] = evaluate(kwExpr.get());

  if (!std::holds_alternative<std::shared_ptr<NinCallable>>(callee))
    throw std::runtime_error(LOC(EXPECT_FN, std::to_string(e.paren.line)));

  auto fn = std::get<std::shared_ptr<NinCallable>>(callee);

  if (!fn->isVariadic() && (int)args.size() != fn->arity()) {
    throw std::runtime_error(LOC(FN_NOT_ENOUGH_ARGS, fn->name(), std::to_string(fn->arity()), std::to_string(args.size()), std::to_string(e.paren.line)));
  }

  return fn->callWithKwargs(std::move(args), std::move(kwargs));
}

Value Interpreter::visit(ArrayExpr &e) {
  std::vector<Value> elements;
  for (const auto &elem : e.elements) {
    elements.push_back(evaluate(elem.get()));
  }
  return std::make_shared<NinArray>(std::move(elements));
}

Value Interpreter::visit(IndexExpr &e) {
  Value obj = evaluate(e.object.get());
  Value idx = evaluate(e.index.get());

  if (std::holds_alternative<std::shared_ptr<NinArray>>(obj)) {
    auto arr = std::get<std::shared_ptr<NinArray>>(obj);
    if (!std::holds_alternative<double>(idx))
      throw std::runtime_error(LOC(ARRAY_INDEX_NAN));
    int i = (int)std::get<double>(idx);
    if (i < 0)
      i = (int)arr->elements.size() + i;
    if (i < 0 || i >= (int)arr->elements.size())
      throw std::runtime_error(LOC(ARRAY_INDEX_OOB, std::to_string(i)));
    return arr->elements[i];
  }

  if (std::holds_alternative<std::string>(obj)) {
    const std::string &s = std::get<std::string>(obj);
    if (!std::holds_alternative<double>(idx))
      throw std::runtime_error(LOC(STRING_INDEX_NAN));
    int i = (int)std::get<double>(idx);
    if (i < 0)
      i = (int)s.size() + i;
    if (i < 0 || i >= (int)s.size())
      throw std::runtime_error(LOC(STRING_INDEX_OOB, std::to_string(i)));
    return std::string(1, s[i]);
  }

  throw std::runtime_error(LOC(TYPE_UNINDEXABLE, std::to_string(e.bracket.line)));
}

Value Interpreter::visit(IndexAssignExpr &e) {
  Value obj = evaluate(e.object.get());
  Value idx = evaluate(e.index.get());
  Value val = evaluate(e.value.get());

  if (auto *v = dynamic_cast<VariableExpr *>(e.object.get())) {
    if (env->isConst(v->name.lexeme))
      throw std::runtime_error(LOC(ASSIGN_CONST_ARRAY, v->name.lexeme));
  }

  if (!std::holds_alternative<std::shared_ptr<NinArray>>(obj))
    throw std::runtime_error(LOC(INDEX_ASSIGN_NOT_ARRAY));
  auto arr = std::get<std::shared_ptr<NinArray>>(obj);
  if (arr->isConst)
    throw std::runtime_error(LOC(MODIFY_CONST_ARRAY));
  if (!std::holds_alternative<double>(idx))
    throw std::runtime_error(LOC(ARRAY_INDEX_NAN));
  int i = (int)std::get<double>(idx);
  if (i < 0)
    i = (int)arr->elements.size() + i;
  if (i < 0 || i >= (int)arr->elements.size())
    throw std::runtime_error(LOC(ARRAY_INDEX_OOB, std::to_string(i)));
  arr->elements[i] = val;
  return val;
}

Value Interpreter::visit(GetExpr &e) {
  Value obj = evaluate(e.object.get());

  if (std::holds_alternative<std::shared_ptr<NinNative>>(obj)) {
    auto native = std::get<std::shared_ptr<NinNative>>(obj);
    return native->getField(e.name.lexeme);
  }

  if (std::holds_alternative<std::shared_ptr<NinModule>>(obj)) {
    auto mod = std::get<std::shared_ptr<NinModule>>(obj);
    auto it = mod->members.find(e.name.lexeme);
    if (it != mod->members.end())
      return it->second;
    throw std::runtime_error(LOC(MODULE_NO_MEMBER, e.name.lexeme));
  }

  if (std::holds_alternative<std::shared_ptr<NinInstance>>(obj)) {
    auto inst = std::get<std::shared_ptr<NinInstance>>(obj);
    auto it = inst->fields.find(e.name.lexeme);
    if (it != inst->fields.end())
      return it->second;
    auto mit = inst->klass->methods.find(e.name.lexeme);
    if (mit != inst->klass->methods.end())
      return makeBoundMethod(inst, mit->second);
    throw std::runtime_error(LOC(UNDEF_PROPERTY, e.name.lexeme));
  }

  throw std::runtime_error(LOC(NOT_GETTABLE, std::to_string(e.name.line)));
}

Value Interpreter::visit(SetExpr &e) {
  Value obj = evaluate(e.object.get());
  Value val = evaluate(e.value.get());

  if (std::holds_alternative<std::shared_ptr<NinNative>>(obj)) {
    auto native = std::get<std::shared_ptr<NinNative>>(obj);
    native->setField(e.name.lexeme, val);
    return val;
  }

  if (std::holds_alternative<std::shared_ptr<NinModule>>(obj)) {
    auto mod = std::get<std::shared_ptr<NinModule>>(obj);
    mod->members[e.name.lexeme] = val;
    return val;
  }

  if (std::holds_alternative<std::shared_ptr<NinInstance>>(obj)) {
    auto inst = std::get<std::shared_ptr<NinInstance>>(obj);
    inst->fields[e.name.lexeme] = val;
    return val;
  }

  throw std::runtime_error(LOC(NOT_SETTABLE, std::to_string(e.name.line)));
}

Value Interpreter::visit(NewExpr &e) {
  Value classVal = evaluate(e.classExpr.get());
  if (!std::holds_alternative<std::shared_ptr<NinClass>>(classVal))
    throw std::runtime_error(LOC(NEW_NOT_CLASS, std::to_string(e.keyword.line)));

  auto klass = std::get<std::shared_ptr<NinClass>>(classVal);
  auto inst = std::make_shared<NinInstance>(klass);

  std::vector<Value> args;
  for (auto &arg : e.arguments)
    args.push_back(evaluate(arg.get()));

  auto it = klass->methods.find("init");
  if (it != klass->methods.end()) {
    auto bound = makeBoundMethod(inst, it->second);
    if ((int)args.size() != bound->arity())
      throw std::runtime_error(LOC(INIT_ARG_MISMATCH, klass->className, std::to_string(bound->arity()), std::to_string(args.size())));
    bound->call(std::move(args));
  } else if (!args.empty()) {
    throw std::runtime_error(LOC(NO_INIT_WITH_ARGS, klass->className));
  }

  return inst;
}

Value Interpreter::visit(ThisExpr &e) { return env->get("this"); }

Value Interpreter::visit(CoroutineExpr &e) {
  Value fnVal = env->get(e.fnName.lexeme);
  if (!std::holds_alternative<std::shared_ptr<NinCallable>>(fnVal))
    throw std::runtime_error(LOC(CORO_NOT_FN, e.fnName.lexeme));

  auto fn = std::get<std::shared_ptr<NinCallable>>(fnVal);
  auto *ninFn = dynamic_cast<NinFunction *>(fn.get());
  if (!ninFn || !ninFn->isAsync)
    throw std::runtime_error(LOC(CORO_NOT_ASYNC, e.fnName.lexeme));

  std::vector<Value> args;
  for (auto &arg : e.arguments)
    args.push_back(evaluate(arg.get()));

  std::unordered_map<std::string, Value> kwargs;
  for (const auto &[kwName, kwExpr] : e.kwargs)
    kwargs[kwName] = evaluate(kwExpr.get());

  auto capturedClosure = ninFn->closure;
  auto capturedDecl = ninFn->decl;

  auto coro = std::make_shared<NinCoroutine>(
      [this, capturedDecl, capturedClosure, args, kwargs]() mutable -> Value {
        auto funcEnv = std::make_shared<Environment>(capturedClosure);
        bindFunctionArgs(this, capturedDecl, funcEnv, args, kwargs);

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
}

Value Interpreter::visit(SuperExpr &e) {
  Value thisVal = env->get("this");
  if (!std::holds_alternative<std::shared_ptr<NinInstance>>(thisVal))
    throw std::runtime_error(LOC(SUPER_OUTSIDE_METHOD));

  auto inst = std::get<std::shared_ptr<NinInstance>>(thisVal);
  auto superclass = inst->klass->superclass;
  if (!superclass)
    throw std::runtime_error(LOC(NO_PARENT_CLASS, inst->klass->className));

  auto it = superclass->methods.find("init");
  if (it == superclass->methods.end())
    throw std::runtime_error(LOC(PARENT_NO_INIT));

  std::vector<Value> args;
  for (auto &arg : e.arguments)
    args.push_back(evaluate(arg.get()));

  auto bound = makeBoundMethod(inst, it->second);
  if ((int)args.size() != bound->arity())
    throw std::runtime_error(LOC(SUPER_ARG_MISMATCH, std::to_string(bound->arity()), std::to_string(args.size())));
  bound->call(std::move(args));
  return std::monostate{};
}

Value Interpreter::visit(AwaitExpr &e) {
  if (!insideCoroutine)
    throw std::runtime_error(LOC(AWAIT_OUTSIDE_CORO, std::to_string(e.keyword.line)));
  return evaluate(e.value.get());
}
