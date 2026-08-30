#pragma once
#include "ast.h"

struct ExprStmt : StmtAcceptor<ExprStmt> {
  ExprPtr expression;
  explicit ExprStmt(ExprPtr expression) : expression(std::move(expression)) {}
};

struct VarDecl : StmtAcceptor<VarDecl> {
  Token name;
  ExprPtr initializer;

  VarDecl(Token name, ExprPtr initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct BlockStmt : StmtAcceptor<BlockStmt> {
  std::vector<StmtPtr> statements;
  explicit BlockStmt(std::vector<StmtPtr> statements)
      : statements(std::move(statements)) {}
};

struct IfStmt : StmtAcceptor<IfStmt> {
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;

  IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

struct WhileStmt : StmtAcceptor<WhileStmt> {
  ExprPtr condition;
  StmtPtr body;

  WhileStmt(ExprPtr condition, StmtPtr body)
      : condition(std::move(condition)), body(std::move(body)) {}
};

struct ForStmt : StmtAcceptor<ForStmt> {
  StmtPtr initializer;
  ExprPtr condition;
  ExprPtr increment;
  StmtPtr body;

  ForStmt(StmtPtr init, ExprPtr cond, ExprPtr inc, StmtPtr body)
      : initializer(std::move(init)), condition(std::move(cond)),
        increment(std::move(inc)), body(std::move(body)) {}
};

struct FunctionStmt : StmtAcceptor<FunctionStmt> {
  Token name;
  std::vector<Token> params;
  std::vector<StmtPtr> body;

  FunctionStmt(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
      : name(std::move(name)), params(std::move(params)),
        body(std::move(body)) {}
};

struct ReturnStmt : StmtAcceptor<ReturnStmt> {
  Token keyword;
  ExprPtr value;

  ReturnStmt(Token keyword, ExprPtr value)
      : keyword(std::move(keyword)), value(std::move(value)) {}
};

struct ClassStmt : StmtAcceptor<ClassStmt> {
  Token name;
  std::vector<std::unique_ptr<FunctionStmt>> methods;
  std::vector<std::unique_ptr<FunctionStmt>> overrides;
  std::unique_ptr<Token> superclass;

  ClassStmt(Token name, std::vector<std::unique_ptr<FunctionStmt>> methods,
            std::vector<std::unique_ptr<FunctionStmt>> overrides,
            std::unique_ptr<Token> superclass)
      : name(std::move(name)), methods(std::move(methods)),
        overrides(std::move(overrides)), superclass(std::move(superclass)) {}
};

struct GlobalDecl : StmtAcceptor<GlobalDecl> {
  Token name;
  ExprPtr initializer;

  GlobalDecl(Token name, ExprPtr initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct ConstDecl : StmtAcceptor<ConstDecl> {
  Token name;
  ExprPtr initializer;

  ConstDecl(Token name, ExprPtr initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct MutexDecl : StmtAcceptor<MutexDecl> {
  Token name;
  ExprPtr initializer;

  MutexDecl(Token name, ExprPtr initializer)
      : name(std::move(name)), initializer(std::move(initializer)) {}
};

struct FreeStmt : StmtAcceptor<FreeStmt> {
  Token name;
  explicit FreeStmt(Token name) : name(std::move(name)) {}
};

struct BreakStmt : StmtAcceptor<BreakStmt> {};
struct ContinueStmt : StmtAcceptor<ContinueStmt> {};
