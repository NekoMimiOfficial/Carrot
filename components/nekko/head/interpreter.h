#pragma once
#include "ast.h"
#include "environment.h"
#include "value.h"
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct ReturnException : public std::runtime_error {
  Value value;
  explicit ReturnException(Value v)
      : std::runtime_error("'return' used outside of a function."),
        value(std::move(v)) {}
};

struct BreakException : std::runtime_error {
  BreakException() : std::runtime_error("'break' used outside of a loop.") {}
};

struct ContinueException : std::runtime_error {
  ContinueException()
      : std::runtime_error("'continue' used outside of a loop.") {}
};

class Interpreter : public StmtVisitor, public ExprVisitor {
public:
  Interpreter(std::string sourceDir = ".", std::vector<std::string> argv = {});
  void interpret(const std::vector<StmtPtr> &statements);

  void executeBlock(const std::vector<StmtPtr> &stmts,
                    std::shared_ptr<Environment> blockEnv);

  void executeOne(Stmt *stmt) { execute(stmt); }
  void execute(Stmt *stmt) { stmt->accept(*this); }
  Value evaluate(Expr *expr) { return expr->accept(*this); }

  std::shared_ptr<Environment> globals;

  void registerBuiltin(std::string name, Value val) {
    globals->define(name, val);
  }

  void registerBuiltinFn(std::shared_ptr<NinCallable> fn) {
    globals->define(fn->name(), fn);
  }

  void registerBuiltinClass(std::shared_ptr<NinClass> classInst) {
    globals->define(classInst->className, classInst);
  }

  void reset(std::string sourceDir, std::vector<std::string> argv = {});

  std::string getSourceDir() { return sourceDir; }
  std::shared_ptr<NinArray> getArgV() { return iargv; }

  Value getCachedModule(const std::string &key);
  void cacheModule(const std::string &key, Value mod);

private:
  static thread_local std::shared_ptr<Environment> env;

  void visit(ExprStmt &s) override;
  void visit(VarDecl &s) override;
  void visit(ConstDecl &s) override;
  void visit(GlobalDecl &s) override;
  void visit(MutexDecl &s) override;
  void visit(BlockStmt &s) override;
  void visit(IfStmt &s) override;
  void visit(WhileStmt &s) override;
  void visit(ForStmt &s) override;
  void visit(FunctionStmt &s) override;
  void visit(AsyncFunctionStmt &s) override;
  void visit(ReturnStmt &s) override;
  void visit(ClassStmt &s) override;
  void visit(FreeStmt &s) override;
  void visit(BreakStmt &s) override;
  void visit(ContinueStmt &s) override;

  Value visit(LiteralExpr &e) override;
  Value visit(VariableExpr &e) override;
  Value visit(AssignExpr &e) override;
  Value visit(UnaryExpr &e) override;
  Value visit(BinaryExpr &e) override;
  Value visit(LogicalExpr &e) override;
  Value visit(CallExpr &e) override;
  Value visit(ArrayExpr &e) override;
  Value visit(IndexExpr &e) override;
  Value visit(IndexAssignExpr &e) override;
  Value visit(GetExpr &e) override;
  Value visit(SetExpr &e) override;
  Value visit(NewExpr &e) override;
  Value visit(ThisExpr &e) override;
  Value visit(SuperExpr &e) override;
  Value visit(CoroutineExpr &e) override;
  Value visit(AwaitExpr &e) override;

  void checkNumberOperand(const Token &op, const Value &val);
  void checkNumberOperands(const Token &op, const Value &left,
                           const Value &right);
  std::string sourceDir;
  std::shared_ptr<NinArray> iargv;

  std::unordered_map<std::string, Value> moduleCache;
  std::mutex moduleCacheMutex;
};

void registerHandler(Interpreter *interp);
