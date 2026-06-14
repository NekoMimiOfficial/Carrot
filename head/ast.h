#pragma once
#include "token.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

struct Expr {
  virtual ~Expr() = default;
};

struct LiteralExpr : Expr {
  std::variant<std::monostate, double, std::string, bool> value;

  explicit LiteralExpr(double d) : value(d) {}
  explicit LiteralExpr(std::string s) : value(std::move(s)) {}
  explicit LiteralExpr(bool b) : value(b) {}
  LiteralExpr() : value(std::monostate{}) {}
};

struct VariableExpr : Expr {
  Token name;
  explicit VariableExpr(Token name) : name(std::move(name)) {}
};

struct BinaryExpr : Expr {
  ExprPtr left;
  Token op;
  ExprPtr right;

  BinaryExpr(ExprPtr left, Token op, ExprPtr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct UnaryExpr : Expr {
  Token op;
  ExprPtr right;

  UnaryExpr(Token op, ExprPtr right)
      : op(std::move(op)), right(std::move(right)) {}
};

struct AssignExpr : Expr {
  Token name;
  ExprPtr value;

  AssignExpr(Token name, ExprPtr value)
      : name(std::move(name)), value(std::move(value)) {}
};

struct CallExpr : Expr {
  ExprPtr callee;
  Token paren;
  std::vector<ExprPtr> arguments;

  CallExpr(ExprPtr callee, Token paren, std::vector<ExprPtr> arguments)
      : callee(std::move(callee)), paren(std::move(paren)),
        arguments(std::move(arguments)) {}
};

struct LogicalExpr : Expr {
  ExprPtr left;
  Token op;
  ExprPtr right;

  LogicalExpr(ExprPtr left, Token op, ExprPtr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct ArrayExpr : Expr {
  std::vector<ExprPtr> elements;
  explicit ArrayExpr(std::vector<ExprPtr> elements)
      : elements(std::move(elements)) {}
};

struct IndexExpr : Expr {
  ExprPtr object;
  ExprPtr index;
  Token bracket;

  IndexExpr(ExprPtr object, ExprPtr index, Token bracket)
      : object(std::move(object)), index(std::move(index)),
        bracket(std::move(bracket)) {}
};

struct IndexAssignExpr : Expr {
  ExprPtr object;
  ExprPtr index;
  ExprPtr value;
  Token bracket;

  IndexAssignExpr(ExprPtr object, ExprPtr index, ExprPtr value, Token bracket)
      : object(std::move(object)), index(std::move(index)),
        value(std::move(value)), bracket(std::move(bracket)) {}
};

struct Stmt {
  virtual ~Stmt() = default;
};

struct ExprStmt : Stmt {
  ExprPtr expression;
  explicit ExprStmt(ExprPtr expression) : expression(std::move(expression)) {}
};

struct PrintStmt : Stmt {
  ExprPtr expression;
  explicit PrintStmt(ExprPtr expression) : expression(std::move(expression)) {}
};

struct VarDecl : Stmt {
  Token name;
  ExprPtr initializer;

  VarDecl(Token name, ExprPtr initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct BlockStmt : Stmt {
  std::vector<StmtPtr> statements;
  explicit BlockStmt(std::vector<StmtPtr> statements)
      : statements(std::move(statements)) {}
};

struct IfStmt : Stmt {
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;

  IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

struct WhileStmt : Stmt {
  ExprPtr condition;
  StmtPtr body;

  WhileStmt(ExprPtr condition, StmtPtr body)
      : condition(std::move(condition)), body(std::move(body)) {}
};

struct ForStmt : Stmt {
  StmtPtr initializer;
  ExprPtr condition;
  ExprPtr increment;
  StmtPtr body;

  ForStmt(StmtPtr init, ExprPtr cond, ExprPtr inc, StmtPtr body)
      : initializer(std::move(init)), condition(std::move(cond)),
        increment(std::move(inc)), body(std::move(body)) {}
};

struct FunctionStmt : Stmt {
  Token name;
  std::vector<Token> params;
  std::vector<StmtPtr> body;

  FunctionStmt(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
      : name(std::move(name)), params(std::move(params)),
        body(std::move(body)) {}
};

struct ReturnStmt : Stmt {
  Token keyword;
  ExprPtr value;

  ReturnStmt(Token keyword, ExprPtr value)
      : keyword(std::move(keyword)), value(std::move(value)) {}
};

struct GetExpr : Expr {
  ExprPtr object;
  Token name;

  GetExpr(ExprPtr object, Token name)
      : object(std::move(object)), name(std::move(name)) {}
};

struct SetExpr : Expr {
  ExprPtr object;
  Token name;
  ExprPtr value;

  SetExpr(ExprPtr object, Token name, ExprPtr value)
      : object(std::move(object)), name(std::move(name)),
        value(std::move(value)) {}
};

struct ClassStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    ClassStmt(Token name,
              std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(std::move(name)), methods(std::move(methods)) {}
};

struct NewExpr : Expr {
    Token keyword;
    Token className;
    std::vector<ExprPtr> arguments;

    NewExpr(Token keyword, Token className, std::vector<ExprPtr> arguments)
        : keyword(std::move(keyword)), className(std::move(className)),
          arguments(std::move(arguments)) {}
};

struct ThisExpr : Expr {
    Token keyword;
    explicit ThisExpr(Token keyword) : keyword(std::move(keyword)) {}
};

struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};
