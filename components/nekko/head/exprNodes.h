#pragma once
#include "ast.h"
#include <cstdint>
#include <string>
#include <variant>

struct LiteralExpr : ExprAcceptor<LiteralExpr> {
  std::variant<std::monostate, double, std::string, bool, uint8_t> value;

  explicit LiteralExpr(double d) : value(d) {}
  explicit LiteralExpr(uint8_t x) : value(x) {}
  explicit LiteralExpr(std::string s) : value(std::move(s)) {}
  explicit LiteralExpr(bool b) : value(b) {}
  LiteralExpr() : value(std::monostate{}) {}
};

struct VariableExpr : ExprAcceptor<VariableExpr> {
  Token name;
  explicit VariableExpr(Token name) : name(std::move(name)) {}
};

struct BinaryExpr : ExprAcceptor<BinaryExpr> {
  ExprPtr left;
  Token op;
  ExprPtr right;

  BinaryExpr(ExprPtr left, Token op, ExprPtr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct UnaryExpr : ExprAcceptor<UnaryExpr> {
  Token op;
  ExprPtr right;

  UnaryExpr(Token op, ExprPtr right)
      : op(std::move(op)), right(std::move(right)) {}
};

struct AssignExpr : ExprAcceptor<AssignExpr> {
  Token name;
  ExprPtr value;

  AssignExpr(Token name, ExprPtr value)
      : name(std::move(name)), value(std::move(value)) {}
};

struct CallExpr : ExprAcceptor<CallExpr> {
  ExprPtr callee;
  Token paren;
  std::vector<ExprPtr> arguments;

  CallExpr(ExprPtr callee, Token paren, std::vector<ExprPtr> arguments)
      : callee(std::move(callee)), paren(std::move(paren)),
        arguments(std::move(arguments)) {}
};

struct LogicalExpr : ExprAcceptor<LogicalExpr> {
  ExprPtr left;
  Token op;
  ExprPtr right;

  LogicalExpr(ExprPtr left, Token op, ExprPtr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct ArrayExpr : ExprAcceptor<ArrayExpr> {
  std::vector<ExprPtr> elements;

  explicit ArrayExpr(std::vector<ExprPtr> elements)
      : elements(std::move(elements)) {}
};

struct IndexExpr : ExprAcceptor<IndexExpr> {
  ExprPtr object;
  ExprPtr index;
  Token bracket;

  IndexExpr(ExprPtr object, ExprPtr index, Token bracket)
      : object(std::move(object)), index(std::move(index)),
        bracket(std::move(bracket)) {}
};

struct IndexAssignExpr : ExprAcceptor<IndexAssignExpr> {
  ExprPtr object;
  ExprPtr index;
  ExprPtr value;
  Token bracket;

  IndexAssignExpr(ExprPtr object, ExprPtr index, ExprPtr value, Token bracket)
      : object(std::move(object)), index(std::move(index)),
        value(std::move(value)), bracket(std::move(bracket)) {}
};

struct GetExpr : ExprAcceptor<GetExpr> {
  ExprPtr object;
  Token name;

  GetExpr(ExprPtr object, Token name)
      : object(std::move(object)), name(std::move(name)) {}
};

struct SetExpr : ExprAcceptor<SetExpr> {
  ExprPtr object;
  Token name;
  ExprPtr value;

  SetExpr(ExprPtr object, Token name, ExprPtr value)
      : object(std::move(object)), name(std::move(name)),
        value(std::move(value)) {}
};

struct SuperExpr : ExprAcceptor<SuperExpr> {
  Token keyword;
  std::vector<ExprPtr> arguments;

  SuperExpr(Token keyword, std::vector<ExprPtr> arguments)
      : keyword(std::move(keyword)), arguments(std::move(arguments)) {}
};

struct NewExpr : ExprAcceptor<NewExpr> {
  Token keyword;
  ExprPtr classExpr;
  std::vector<ExprPtr> arguments;

  NewExpr(Token keyword, ExprPtr classExpr, std::vector<ExprPtr> arguments)
      : keyword(std::move(keyword)), classExpr(std::move(classExpr)),
        arguments(std::move(arguments)) {}
};

struct ThisExpr : ExprAcceptor<ThisExpr> {
  Token keyword;
  explicit ThisExpr(Token keyword) : keyword(std::move(keyword)) {}
};

struct CoroutineExpr : ExprAcceptor<CoroutineExpr> {
  Token keyword;
  Token fnName;
  std::vector<ExprPtr> arguments;

  CoroutineExpr(Token keyword, Token fnName, std::vector<ExprPtr> arguments)
      : keyword(std::move(keyword)), fnName(std::move(fnName)),
        arguments(std::move(arguments)) {}
};

struct AwaitExpr : ExprAcceptor<AwaitExpr> {
  Token keyword;
  ExprPtr value;

  AwaitExpr(Token keyword, ExprPtr value)
      : keyword(std::move(keyword)), value(std::move(value)) {}
};
