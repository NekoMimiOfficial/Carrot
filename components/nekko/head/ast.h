#pragma once
#include "token.h"
#include "value.h"
#include "visitor.h"
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
  virtual Value accept(ExprVisitor &v) = 0;
};

struct Stmt {
  virtual ~Stmt() = default;
  virtual void accept(StmtVisitor &v) = 0;
};

template <typename Derived> struct ExprAcceptor : Expr {
  Value accept(ExprVisitor &v) override {
    return v.visit(static_cast<Derived &>(*this));
  }
};

template <typename Derived> struct StmtAcceptor : Stmt {
  void accept(StmtVisitor &v) override {
    v.visit(static_cast<Derived &>(*this));
  }
};

#include "exprNodes.h"
#include "stmtNodes.h"

struct AsyncFunctionStmt : FunctionStmt {
  AsyncFunctionStmt(Token name, std::vector<Token> params,
                    std::vector<StmtPtr> body)
      : FunctionStmt(std::move(name), std::move(params), std::move(body)) {}
  void accept(StmtVisitor &v) override { v.visit(*this); }
};
