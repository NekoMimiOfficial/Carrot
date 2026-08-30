#pragma once
#include "value.h"

struct ExprStmt;
struct VarDecl;
struct ConstDecl;
struct GlobalDecl;
struct MutexDecl;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct FunctionStmt;
struct AsyncFunctionStmt;
struct ReturnStmt;
struct ClassStmt;
struct FreeStmt;
struct BreakStmt;
struct ContinueStmt;

struct LiteralExpr;
struct VariableExpr;
struct AssignExpr;
struct UnaryExpr;
struct BinaryExpr;
struct LogicalExpr;
struct CallExpr;
struct ArrayExpr;
struct IndexExpr;
struct IndexAssignExpr;
struct GetExpr;
struct SetExpr;
struct NewExpr;
struct ThisExpr;
struct SuperExpr;
struct CoroutineExpr;
struct AwaitExpr;

struct StmtVisitor {
  virtual ~StmtVisitor() = default;
  virtual void visit(ExprStmt &s) = 0;
  virtual void visit(VarDecl &s) = 0;
  virtual void visit(ConstDecl &s) = 0;
  virtual void visit(GlobalDecl &s) = 0;
  virtual void visit(MutexDecl &s) = 0;
  virtual void visit(BlockStmt &s) = 0;
  virtual void visit(IfStmt &s) = 0;
  virtual void visit(WhileStmt &s) = 0;
  virtual void visit(ForStmt &s) = 0;
  virtual void visit(FunctionStmt &s) = 0;
  virtual void visit(AsyncFunctionStmt &s) = 0;
  virtual void visit(ReturnStmt &s) = 0;
  virtual void visit(ClassStmt &s) = 0;
  virtual void visit(FreeStmt &s) = 0;
  virtual void visit(BreakStmt &s) = 0;
  virtual void visit(ContinueStmt &s) = 0;
};

struct ExprVisitor {
  virtual ~ExprVisitor() = default;
  virtual Value visit(LiteralExpr &e) = 0;
  virtual Value visit(VariableExpr &e) = 0;
  virtual Value visit(AssignExpr &e) = 0;
  virtual Value visit(UnaryExpr &e) = 0;
  virtual Value visit(BinaryExpr &e) = 0;
  virtual Value visit(LogicalExpr &e) = 0;
  virtual Value visit(CallExpr &e) = 0;
  virtual Value visit(ArrayExpr &e) = 0;
  virtual Value visit(IndexExpr &e) = 0;
  virtual Value visit(IndexAssignExpr &e) = 0;
  virtual Value visit(GetExpr &e) = 0;
  virtual Value visit(SetExpr &e) = 0;
  virtual Value visit(NewExpr &e) = 0;
  virtual Value visit(ThisExpr &e) = 0;
  virtual Value visit(SuperExpr &e) = 0;
  virtual Value visit(CoroutineExpr &e) = 0;
  virtual Value visit(AwaitExpr &e) = 0;
};
