#pragma once
#include "ast.h"
#include "environment.h"
#include "value.h"
#include <memory>
#include <vector>

struct ReturnException {
  Value value;
  explicit ReturnException(Value v) : value(std::move(v)) {}
};

struct BreakException {};
struct ContinueException {};

class Interpreter {
public:
  Interpreter();
  Interpreter(std::string sourceDir = ".", std::vector<std::string> argv = {});
  void interpret(const std::vector<StmtPtr> &statements);

  void executeBlock(const std::vector<StmtPtr> &stmts,
                    std::shared_ptr<Environment> blockEnv);

  void executeOne(Stmt *stmt) { execute(stmt); }

  std::shared_ptr<Environment> globals;

  void registerBuiltin(std::string name, Value val)
  { globals->define(name, val); }

  void registerBuiltinFn(std::shared_ptr<NinCallable> fn)
  { globals->define(fn->name(), fn); }

  void registerBuiltinClass(std::shared_ptr<NinClass> classInst)
  { globals->define(classInst->className, classInst); }

  void reset(std::string sourceDir, std::vector<std::string> argv = {});

private:
  std::shared_ptr<Environment> env;

  void execute(Stmt *stmt);
  Value evaluate(Expr *expr);

  void checkNumberOperand(const Token &op, const Value &val);
  void checkNumberOperands(const Token &op, const Value &left,
                           const Value &right);
};
